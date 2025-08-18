
#include <iostream>
#include <thread>// 쓰레드
using namespace std;

#pragma comment(lib, "ws2_32.lib") //라이브러리 블러오기
#include <winsock2.h> //소켓 서버 만들기 위해 필요
#include<WS2tcpip.h>

enum IO_TYPE
{
    NONE,
    SEND,
    RECV
};


struct Session
{
    WSAOVERLAPPED overlappend = {};
    //소켓
    SOCKET socket = INVALID_SOCKET;
    IO_TYPE type;
    char recvBuffer[512] = {};
    char sendBuffer[512] = {};
};

    
//또다른 쓰레드 만들어서 이 함수를 돌리기 위해서
void SendRecvThread(HANDLE _iocpHandle)
{

    DWORD byteTransferred = 0;
    ULONG_PTR  key = 0;
    Session* session = nullptr;


    while (true)
    {
        printf("Watting...\n");
        
        //IOCP에서 작업이 완료될 때까지 대기
        GetQueuedCompletionStatus(_iocpHandle, &byteTransferred, &key, (LPOVERLAPPED*)&session, INFINITE);




        //수신 버퍼 및 기타 정보를 설정하여 다시 데이터 수신 준비
        WSABUF wsaBuf;
        wsaBuf.buf = session->recvBuffer;
        wsaBuf.len = sizeof(session->recvBuffer); // 받을 메모리 크기

        DWORD dateBufferlen = 0;
        DWORD flags = 0;


        switch (session->type)
        {
        case SEND:
            printf("SEND : %s\n", session->sendBuffer);
            session->type = RECV;
            //비동기 수신을 디시 시작, 지속저그올 데이터 수선을 위해 반복
            WSARecv(session->socket, OUT & wsaBuf, 1, OUT & dateBufferlen, &flags, &session->overlappend, NULL);

            break;
        case RECV:
            printf("Recv : %s\n", session->recvBuffer);
            session->type = SEND;
            //비동기 수신을 디시 시작, 지속저그올 데이터 수선을 위해 반복
            WSASend(session->socket, &wsaBuf, 1, &dateBufferlen, flags, &session->overlappend, NULL);

            break;

        }



        this_thread::sleep_for(1s);
    }
}

int main()
{
    printf("-----------Server----------\n");
    
    // 서버가 사용할 수 있는 windows 소캣 사양의 버전 설정
    WORD wVersionRequested;

    //windows 소캣 구현에 대한 세부 정보 받기 위해 사용
    WSADATA wsaData;

    //버전을 만들어서 넣어줌
    wVersionRequested = MAKEWORD(2,2);

    //winsock DLL 사용을 시작
    if (WSAStartup(wVersionRequested, &wsaData) != 0)
    {
        //에러 뱉어 내고 끝
        printf("winsock filed with error\n");
        return 1;
    }

    //AF_INET : IPv4(인터넷 프로토콜 버전 4) 주소 패밀리입니다.
    //AF_INET6 : IPv6(인터넷 프로토콜 버전 6) 주소 패밀리입니다.
   
    //SOCK_STREAM : TCP
    //SOCK_DGRAM : UDP

    //소켓
    //프로토콜  : 0 값이 0인 경우 자동으로 할당
    SOCKET listentSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listentSocket == INVALID_SOCKET)
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
    
    //정보 : 서버 주소
    //sockaddr_in 구조체 연동
    //SOCKADDR_IN service  = {}; 0으로 초기화 해도됨
    SOCKADDR_IN service;
    //memset(시작주소,값,크기);
    //0 으로 해당 구조체(service)초기화
    memset(&service,0,sizeof(service));
    //address family : IPv4
    service.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &service.sin_addr);
    service.sin_port = htons(5500);

    //(socket)전화기 <- 정보(service) 등록
    //bind(소캣,주소,정보의 크기)
    if (bind(listentSocket, (SOCKADDR*)&service,sizeof(service)) == SOCKET_ERROR)
    {
        //변환값이 SOCKET_ERROR 에러라면 에러 코드 확인
        printf("bind error : %d\n", WSAGetLastError());

        //만들 소켓 닫고
        closesocket(listentSocket);
        //winsock DLL 종료
        WSACleanup();
        //프로그램 종료
        return 1;
    }

    //listen에 바인드된 소켓 & 몇명 대기 할지 설정
    if (listen(listentSocket, 10) == SOCKET_ERROR)
    {
        //에러 코드 확인
        printf("listen error : %d\n", WSAGetLastError());

        //만들 소켓 닫고
        closesocket(listentSocket);
        //winsock DLL 종료
        WSACleanup();
        //프로그램 종료
        return 1;
    }

    printf("listening...\n");
    
    
    //CreateIoCompletionPort 만드는거 뿐만 아니라
    //CreateIoCompletionPort 소켓하고 iocpHandle 등록할때도 사용
    //매개변수의 차이로 인해 만드는거와 등록
    // HANDLE iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL, NULL, NULL); 관찰할 수 있는 iocpHandle 만듬
    HANDLE iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL, NULL, NULL);

    thread recvThread(SendRecvThread, iocpHandle);

    //프로그램 종료 하지 않게
    while (true)
    {


        SOCKET acceptSocket = accept(listentSocket, NULL, NULL);

        if (acceptSocket == INVALID_SOCKET)
        {
            printf("listen accept error : %d\n", WSAGetLastError());

            closesocket(listentSocket);
            WSACleanup();

            return 1;
        }

        //연결된 상태
        printf("Client acceptn\n");
        ULONG_PTR key = 0;
        //클라이언트 소캣을 iocpHandle과 연결
        CreateIoCompletionPort((HANDLE)acceptSocket,iocpHandle, key,0);

        Session* session = new Session;
        session->socket = acceptSocket;
        session->type = RECV;
        //수선 버퍼 설정
        WSABUF wsaBuf;
        //공간의 주소 넣어줌 //받을 메모리 공간
        wsaBuf.buf = session->recvBuffer;
        //recvBuffer크기는 현재 512
        wsaBuf.len = sizeof(session->recvBuffer); // 받을 메모리 크기

        //수신된 데ㅐ이터 길이을 저장할 변수
        DWORD recvlen = 0;
        //플래그 변수 : 현재는 사용하지 않음
        DWORD flags = 0;
        //WSAOVERLAPPED 구조체 할당 후 초기화 비동가 I/O 작업에 사용
        //WSAOVERLAPPED overlappend = {};



        //WSARecv : s 소켓, 위 acceptSocket 넣어주면 됨
        //ipbuffers :WSABUF 구조제 배열에 대한 포인터
        //dwBufferCount : WSABUF 구조체 갯수
        //ipNumberOfBytesRecvd : 수신 완료 후 받을 바이트에 대한 포인터
        //ipflage : 플래그,
        //ipOverlapped : WSAOVERLAPPED 구조체에 대한 포인터
        //ipComPletIonRoution : 수신 완료 후 호출되는 콜백 함수
        printf("main session->overlappend : %p\n", session);
        WSARecv(acceptSocket, OUT &wsaBuf,1, OUT &recvlen, &flags,&session->overlappend,NULL);

    }
    
    //스레드가 다 끝날때까지 기다리는 
    recvThread.join();
    closesocket(listentSocket);
    //winsock DLL 종료
    WSACleanup();

    return 0;
}

