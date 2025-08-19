#include "pch.h"
#include "Listener.h"
#include "Service.h"

Listener::Listener()
{
	HANDLE iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);

}

Listener::~Listener()
{
	closesocket(socket);
	socket = INVALID_SOCKET;
}

bool Listener::StartAccept(Service& service)
{

#pragma region 받는 소켓 만들기
	socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (socket == INVALID_SOCKET)
	{
		printf("Listener sock error : %d\n", WSAGetLastError());

		return false;
	}
#pragma endregion 

#pragma region 서버 주소와 받은 소켓 연결
	if (bind(socket, (SOCKADDR*)&service.GetSockAddr(), sizeof(service.GetSockAddr())) == SOCKET_ERROR)
	{
		//변환값이 SOCKET_ERROR 에러라면 에러 코드 확인
		printf("Listener bind error : %d\n", WSAGetLastError());

		return false;
	}
#pragma endregion 

#pragma region 받을 준비 하기
	if (listen(socket, SOMAXCONN) == SOCKET_ERROR)
	{
		//에러 코드 확인
		printf("Listener listen error : %d\n", WSAGetLastError());
		return false;
	}

#pragma endregion 
	printf("listening...\n");
#pragma region 비동기 Accept 함수 만듬
	DWORD dwBytes;
	//ConnnentEX 함수포인터 모드
	LPFN_ACCEPTEX lpfnAcceptEx = nullptr;
	GUID guidAcceptEx = WSAID_ACCEPTEX;

	if (WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidAcceptEx, sizeof(guidAcceptEx)
		, &lpfnAcceptEx, sizeof(lpfnAcceptEx), &dwBytes, NULL, NULL) == SOCKET_ERROR)
	{
		printf("WSAIoctl ConnectEx error : %d\n", WSAGetLastError());
		return false;
	}
	
#pragma endregion 

#pragma region 빈 accepat 용 소켓 만들기
	SOCKET acceptSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (acceptSocket == INVALID_SOCKET)
	{
		//에러 발생시 어떤 에러가 발생 했는지 확인
		printf("acceptSocket error :%d\n", WSAGetLastError());
		return false;
	}
	else
	{
		// 성공적으로 소캣 성생 출력
		printf("acceptSocket creation\n");
	}
#pragma endregion 

#pragma region IOCP 핸들 관찰자 생성  
	ULONG_PTR key = 0;
	CreateIoCompletionPort((HANDLE)socket, iocpHandle, key, 0);
#pragma endregion 

#pragma region 비동기 accept 함수 호출
	char accpetBuffer[1024];
	WSAOVERLAPPED overlapped = {};
	//함수포인터를 통해 연결
	if (lpfnAcceptEx(socket, acceptSocket, accpetBuffer, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwBytes,
		&overlapped) == FALSE)
	{
		//에러 코드가 ERROR_IO_PENDING
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			printf("sever lpfnAcceptEx error : %d\n", WSAGetLastError());
			return false;
		}
	}
#pragma endregion 
	return false;
}

void Listener::CloseSocket()
{
	if (socket != INVALID_SOCKET)
	{
		closesocket(socket);
		socket = INVALID_SOCKET;
	}

}
