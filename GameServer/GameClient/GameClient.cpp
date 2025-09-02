
#include <iostream>
#include <thread>// 쓰레드
using namespace std;
#pragma comment(lib, "ws2_32.lib") //라이브러리 블러오기
#include <winsock2.h> //소켓 서버 만들기 위해 필요
#include<WS2tcpip.h>
#include<MSWSock.h>


void ConnectThread(HANDLE _iocpHandle)
{

    DWORD bytesTransferred = 0;
    ULONG_PTR key = 0;
    WSAOVERLAPPED overlapped = {};


    while (true)
    {


        printf("GameClient watting..\n");


        if (GetQueuedCompletionStatus(_iocpHandle, &bytesTransferred, &key, (LPOVERLAPPED*)&overlapped, INFINITE))
        {
            printf("Connect successed\n"); // 연결 성공 메세지 출력
        }
        this_thread::sleep_for(1s);
    }
}


int main()
{
	printf("-------GameClient--------\n");
#pragma region Win Socket 시작

    this_thread::sleep_for(1s);

    WORD wVersionRequested;
    //windows 소캣 구현에 대한 세부 정보 받기 위해 사용
    WSADATA wsaData;
    //버전을 만들어서 넣어줌
    wVersionRequested = MAKEWORD(2, 2);
    //winsock DLL 사용을 시작
    if (WSAStartup(wVersionRequested, &wsaData) != 0)
    {
        //에러 뱉어 내고 끝
        printf("winsock filed with error\n");
        return 1;
    }


#pragma endregion 

#pragma region Win Socket 만들기
    //이전 소켓 만들기
    //    SOCKET connectSocket = socket(AF_INET, SOCK_STREAM, 0);
    //소켓만들기
    SOCKET connectSocket = WSASocket(AF_INET,SOCK_STREAM,IPPROTO_TCP,NULL,0,WSA_FLAG_OVERLAPPED);
   
    if (connectSocket == INVALID_SOCKET)
    {
        printf("GameClient sock error : %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }


#pragma endregion 

#pragma region 내 주소 만들기

    //클라이언트 주소
    SOCKADDR_IN clientSevice;
    memset(&clientSevice, 0, sizeof(clientSevice));
    clientSevice.sin_family = AF_INET;
    clientSevice.sin_addr.s_addr = htonl(INADDR_ANY); // 내꺼 아무거나;
    clientSevice.sin_port = htons(0); // 임의 포트 설정 아무거나

#pragma endregion 

#pragma region 내 소켓이 내 주소 연결
    //클라이언트 로컬 주소와 connectSocket의 바인딩 필요 == 서버와 유가
    if (bind(connectSocket, (SOCKADDR*)&clientSevice, sizeof(clientSevice)) == SOCKET_ERROR)
    {
        printf("Client connectSocket,clientSevice bind error : %d\n", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
        return 1;
    }
#pragma endregion 

#pragma region
    //ICOP 핸들 생성
    HANDLE iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);
    ULONG_PTR key = 0;
    CreateIoCompletionPort((HANDLE)connectSocket, iocpHandle, key, 0);

    //스레드 실행
    thread t(ConnectThread, iocpHandle);

#pragma endregion 


   DWORD dwBytes;

#pragma region 비동기 Connect 함수 만듬

    //ConnnentEX 함수포인터 모드
    LPFN_CONNECTEX lpfnConnectEx = nullptr;
    GUID guidConnectEX = WSAID_CONNECTEX;

    if (WSAIoctl(connectSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidConnectEX, sizeof(guidConnectEX)
        , &lpfnConnectEx, sizeof(lpfnConnectEx), &dwBytes, NULL, NULL) == SOCKET_ERROR)
    {
        printf("WSAIoctl ConnectEx error : %d\n", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
        return 1;
    }
#pragma endregion 

#pragma region 비동기 DisConnect 함수 만듬

    //DisConnnentEX 함수포인터 모드
    LPFN_DISCONNECTEX lpfnDisConnectEx = nullptr;
    //GUID도 disconnect용으로
    GUID guidDisConnectEX = WSAID_DISCONNECTEX;

    if (WSAIoctl(connectSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidDisConnectEX, sizeof(guidDisConnectEX)
        , &lpfnDisConnectEx, sizeof(lpfnDisConnectEx), &dwBytes, NULL, NULL) == SOCKET_ERROR)
    {
        printf("WSAIoctl DisConnectEx error : %d\n", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
        return 1;
    }
#pragma endregion 

#pragma region 접속할 서버 주소 만들기
    //서버주소
    SOCKADDR_IN serverService;
    memset(&serverService, 0, sizeof(serverService));
    serverService.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &serverService.sin_addr);
    serverService.sin_port = htons(5500);
#pragma endregion 

    DWORD bytesTransferred = 0;
    WSAOVERLAPPED overlapped = {};
   
#pragma region 비동기 Connect 함수 콜 == 연결해줘

    //connect
    if (lpfnConnectEx(connectSocket, (SOCKADDR*)&serverService, sizeof(serverService), nullptr, 0, &bytesTransferred, &overlapped) == FALSE)
    {
        //에러 코드가 ERROR_IO_PENDING
        if (WSAGetLastError() != ERROR_IO_PENDING)
        {
            printf("Client lpfnConnectEx error : %d\n", WSAGetLastError());
            closesocket(connectSocket);
            WSACleanup();
            return 1;
        }
    }

#pragma endregion 



#pragma region 비동기 disConnect 함수 콜 == 연결 끊어줘

    //함수 포인터를 통해서 함수 콜
    if (lpfnDisConnectEx(connectSocket, &overlapped,0,0) == FALSE)
    {
        //에러 코드가 ERROR_IO_PENDING
        if (WSAGetLastError() != ERROR_IO_PENDING)
        {
            printf("Client lpfnConnectEx error : %d\n", WSAGetLastError());
            closesocket(connectSocket);
            WSACleanup();
            return 1;
        }
    }

#pragma endregion 


    t.join();
    closesocket(connectSocket);

    WSACleanup();

    return 1;
}

