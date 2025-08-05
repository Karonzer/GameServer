// GameServer.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
using namespace std;

#pragma comment(lib, "ws2_32.lib") //라이브러리 블러오기
#include <winsock2.h> //소켓 서버 만들기 위해 필요
#include<WS2tcpip.h>

int main()
{
    std::cout << "Game Server Hello World!\n";
    
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
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
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
   // service.sin_addr.s_addr = inet_addr("127.0.0.1");
    inet_pton(AF_INET, "127.0.0.1", &service);
    service.sin_port = htons(27015);

    //(socket)전화기 <- 정보(service) 등록
    //bind(소캣,주소,정보의 크기)
    if (bind(sock, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
    {
        //변환값이 SOCKET_ERROR 에러라면 에러 코드 확인
        printf("bind error : %d\n", WSAGetLastError());

        //만들 소켓 닫고
        closesocket(sock);
        //winsock DLL 종료
        WSACleanup();
        //프로그램 종료
        return 1;
    }

    if (listen(sock, 10) == SOCKET_ERROR)
    {
        //에러 코드 확인
        printf("listen error : %d\n", WSAGetLastError());

        //만들 소켓 닫고
        closesocket(sock);
        //winsock DLL 종료
        WSACleanup();
        //프로그램 종료
        return 1;
    }

    //프로그램 종료 하지 않게
    while (true)
    {
        printf("listening...\n");

        //클라이언트 접속시
        //accept(소켓, 클라주소 담은 구조체, 구조체 크기)
        accept(sock,NULL,NULL);//클라이언트가 접속 할때까지 대기

        printf("Accept!!\n");
    }

    closesocket(sock);
    //winsock DLL 종료
    WSACleanup();
}

