
#include <iostream>
#include <thread>// 쓰레드
using namespace std;
#pragma comment(lib, "ws2_32.lib") //라이브러리 블러오기
#include <winsock2.h> //소켓 서버 만들기 위해 필요
#include<WS2tcpip.h>
#include<MSWSock.h>


    
//또다른 쓰레드 만들어서 이 함수를 돌리기 위해서
void AcceptThread(HANDLE _iocpHandle)
{

    DWORD bytesTransferred = 0;
    ULONG_PTR key = 0;
    WSAOVERLAPPED overlapped = {};


    while (true)
    {


        printf("GameServer watting..\n");


        if (GetQueuedCompletionStatus(_iocpHandle, &bytesTransferred, &key, (LPOVERLAPPED*)&overlapped, INFINITE))
        {
            printf("GameServer Clinet successed\n"); // 연결 성공 메세지 출력
        }
        this_thread::sleep_for(1s);
    }
}

int main()
{
    printf("-----------Server----------\n");
#pragma region Win Socket 시작
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
#pragma endregion 

#pragma region 받는 소켓 만들기
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
#pragma endregion 
    
#pragma region 서버 주소 만들기
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

#pragma endregion 

#pragma region 서버 주소와 받은 소켓 연결
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
#pragma endregion 

#pragma region 받을 준비 하기
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
#pragma endregion

    printf("listening...\n");
    
#pragma region IOCP 핸들 생성
    //CreateIoCompletionPort 만드는거 뿐만 아니라
    //CreateIoCompletionPort 소켓하고 iocpHandle 등록할때도 사용
    //매개변수의 차이로 인해 만드는거와 등록
    // HANDLE iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL, NULL, NULL); 관찰할 수 있는 iocpHandle 만듬
    HANDLE iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL, NULL, NULL);
    ULONG_PTR key = 0;
    CreateIoCompletionPort((HANDLE)listentSocket, iocpHandle, key, 0);
    thread t(AcceptThread, iocpHandle);
#pragma endregion

#pragma region 비동기 Accept 함수 만듬
    DWORD dwBytes;
    //ConnnentEX 함수포인터 모드
    LPFN_ACCEPTEX lpfnAcceptEx = nullptr;
    GUID guidAcceptEx = WSAID_ACCEPTEX;

    if (WSAIoctl(listentSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidAcceptEx, sizeof(guidAcceptEx)
        , &lpfnAcceptEx, sizeof(lpfnAcceptEx), &dwBytes, NULL, NULL) == SOCKET_ERROR)
    {
        printf("WSAIoctl ConnectEx error : %d\n", WSAGetLastError());
        closesocket(listentSocket);
        WSACleanup();
        return 1;
    }
#pragma endregion 


#pragma region 빈 accepat 용 소켓 만들기
    SOCKET acceptSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (acceptSocket == INVALID_SOCKET)
    {
        //에러 발생시 어떤 에러가 발생 했는지 확인
        printf("acceptSocket error :%d\n", WSAGetLastError());
        closesocket(listentSocket);
        WSACleanup();
        return 1;
    }
    else
    {
        // 성공적으로 소캣 성생 출력
        printf("acceptSocket creation\n");
    }
#pragma endregion 

#pragma region 비동기 accept 함수 호출

    char accpetBuffer[1024];
    WSAOVERLAPPED overlapped = {};
    //함수포인터를 통해 연결
    if (lpfnAcceptEx(listentSocket, acceptSocket, accpetBuffer, 0, sizeof(SOCKADDR_IN)+16, sizeof(SOCKADDR_IN) + 16,&dwBytes,
        &overlapped )== FALSE)
    {
        //에러 코드가 ERROR_IO_PENDING
        if (WSAGetLastError() != ERROR_IO_PENDING)
        {
            printf("sever lpfnAcceptEx error : %d\n", WSAGetLastError());
            closesocket(listentSocket);
            WSACleanup();
            return 1;
        }
    }
#pragma endregion 




    //스레드가 다 끝날때까지 기다리는 
    t.join();
    closesocket(listentSocket);
    //winsock DLL 종료
    WSACleanup();

    return 0;
}

