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

#pragma region �޴� ���� �����
	socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (socket == INVALID_SOCKET)
	{
		printf("Listener sock error : %d\n", WSAGetLastError());

		return false;
	}
#pragma endregion 

#pragma region ���� �ּҿ� ���� ���� ����
	if (bind(socket, (SOCKADDR*)&service.GetSockAddr(), sizeof(service.GetSockAddr())) == SOCKET_ERROR)
	{
		//��ȯ���� SOCKET_ERROR ������� ���� �ڵ� Ȯ��
		printf("Listener bind error : %d\n", WSAGetLastError());

		return false;
	}
#pragma endregion 

#pragma region ���� �غ� �ϱ�
	if (listen(socket, SOMAXCONN) == SOCKET_ERROR)
	{
		//���� �ڵ� Ȯ��
		printf("Listener listen error : %d\n", WSAGetLastError());
		return false;
	}

#pragma endregion 
	printf("listening...\n");
#pragma region �񵿱� Accept �Լ� ����
	DWORD dwBytes;
	//ConnnentEX �Լ������� ���
	LPFN_ACCEPTEX lpfnAcceptEx = nullptr;
	GUID guidAcceptEx = WSAID_ACCEPTEX;

	if (WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidAcceptEx, sizeof(guidAcceptEx)
		, &lpfnAcceptEx, sizeof(lpfnAcceptEx), &dwBytes, NULL, NULL) == SOCKET_ERROR)
	{
		printf("WSAIoctl ConnectEx error : %d\n", WSAGetLastError());
		return false;
	}
	
#pragma endregion 

#pragma region �� accepat �� ���� �����
	SOCKET acceptSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (acceptSocket == INVALID_SOCKET)
	{
		//���� �߻��� � ������ �߻� �ߴ��� Ȯ��
		printf("acceptSocket error :%d\n", WSAGetLastError());
		return false;
	}
	else
	{
		// ���������� ��Ĺ ���� ���
		printf("acceptSocket creation\n");
	}
#pragma endregion 

#pragma region IOCP �ڵ� ������ ����  
	ULONG_PTR key = 0;
	CreateIoCompletionPort((HANDLE)socket, iocpHandle, key, 0);
#pragma endregion 

#pragma region �񵿱� accept �Լ� ȣ��
	char accpetBuffer[1024];
	WSAOVERLAPPED overlapped = {};
	//�Լ������͸� ���� ����
	if (lpfnAcceptEx(socket, acceptSocket, accpetBuffer, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwBytes,
		&overlapped) == FALSE)
	{
		//���� �ڵ尡 ERROR_IO_PENDING
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
