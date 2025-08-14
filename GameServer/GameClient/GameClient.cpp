
#include <iostream>
#include <thread>// 쓰레드
using namespace std;
#pragma comment(lib, "ws2_32.lib") //라이브러리 블러오기
#include <winsock2.h> //소켓 서버 만들기 위해 필요
#include<WS2tcpip.h>



struct Session
{
    WSAOVERLAPPED overlappend = {}; //비동기 I/O 작업을 위한 구조체
    //소켓
    SOCKET socket = INVALID_SOCKET; //클라이언트와의 통신을 담당하는 소켓

    char sendBuffer[512] = {}; // 데이토 송신을 위한 버퍼
};

void SendThread(HANDLE _iocpHandle)
{
    DWORD byteTransferred = 0;
    ULONG_PTR  key = 0;
    Session* snedSession = nullptr;

    while (true)
    {
        printf("Client Watting..\n");

        //보낼 준비가 돤다면 보내고 넘어감
        if (GetQueuedCompletionStatus(_iocpHandle, &byteTransferred, &key, (LPOVERLAPPED*)&snedSession, INFINITE))
        {
            WSABUF wasBuf;
            wasBuf.buf = snedSession->sendBuffer;
            wasBuf.len = sizeof(snedSession->sendBuffer);

            DWORD sendlen = 0;
            DWORD flags = 0;


            printf("Send : %s\n", snedSession->sendBuffer);

            if (WSASend(snedSession->socket, &wasBuf, 1, &sendlen, flags, &snedSession->overlappend, NULL))
            {
                printf("Send Error: %d\n", WSAGetLastError());
                break;
            }
        }




        this_thread::sleep_for(1s);
    }
}

int main()
{
	printf("-------GameClient--------\n");
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

    SOCKET connectSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (connectSocket == INVALID_SOCKET)
    {
        //에러 발생시 어떤 에러가 발생 했는지 확인
        printf("socket error :%d\n", WSAGetLastError());
        return 1;
    }
    else
    {
        // 성공적으로 소캣 성생 출력
        printf("socket creation\n");
    }

    SOCKADDR_IN service;
    memset(&service, 0, sizeof(service));
    service.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &service.sin_addr);
    service.sin_port = htons(5500);

    //서버에 접속 service(접속할 서버 정보)
    //connect(내 소켓, 서버의 주소, 해당 주소의 구조체 크기)
    if (connect(connectSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
    {
        printf("connect function error : %d\n", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
        return 1;
    }

    printf("connect to service\n");

    //iocpHandle 핸들 만듬
    HANDLE iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);
    //스레드 시작
    thread t(SendThread,iocpHandle);

    // 키값 설정
    ULONG_PTR key = 0;
    //iocpHandle과 소켓 연결
    CreateIoCompletionPort((HANDLE)connectSocket, iocpHandle, key, 0);

    //Session할당
    Session* session = new Session;
    //Session의 소켓은 connectSocket
    session->socket = connectSocket;

    char snedBuffer[512] = "Hello this is Client"; // 전송할 메세지 설정
    //session->sendBuffer에 보내고 싶은 자료 복사
    //memccpy(담은 공간의 주소, 복사할 공간 주소, 복사할 크기)
    memcpy(session->sendBuffer, snedBuffer,sizeof(snedBuffer));

    WSABUF wasBuf;
    wasBuf.buf = session->sendBuffer;
    wasBuf.len = sizeof(session->sendBuffer);

    DWORD snedlen = 0;
    DWORD flags = 0;

    //WSASend
    WSASend(connectSocket, &wasBuf, 1, &snedlen, flags, &session->overlappend, NULL);

    //스레드가 종료할때 까지 대기
    t.join();
    

    //while (true)
    //{

    //    char sendBuffer[] = "hello this is Client";

    //    //send(클라랑 연결된 소캣, 보낼버퍼,크기,0(flags));
    //    if (send(connectSocket, sendBuffer, sizeof(sendBuffer), 0) == SOCKET_ERROR)
    //    {
    //        //에러 코드 검사
    //        printf("Send Error : %d\n : ", WSAGetLastError());
    //        //클라이언트와 연결된 소켓 닫기
    //        closesocket(connectSocket);
    //        //처음으로 다시
    //        break;
    //    }

    //    Sleep(1000);
    //    //엔터를 첬을때 경우 연결 종료
    //    if (GetAsyncKeyState(VK_RETURN))
    //    {
    //        //연결 끊기
    //        //SD_RECEIVE(0) recv를 막는다 . 수신을 막는다
    //        //SD_SEND(1) send를 막는다, 송신을 막는다
    //        //SD_BOTH 둘다 막는다, 송수신믈 막는다
    //        shutdown(connectSocket, SD_BOTH);
    //        break;
    //    }


    //}
    closesocket(connectSocket);
    WSACleanup();

    return 1;
}

