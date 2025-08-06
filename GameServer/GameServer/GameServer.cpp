
#include <iostream>
using namespace std;

#pragma comment(lib, "ws2_32.lib") //라이브러리 블러오기
#include <winsock2.h> //소켓 서버 만들기 위해 필요
#include<WS2tcpip.h>

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

    //클라이언트의 정보를 담기 위해
    SOCKADDR_IN clientService;
    //SOCKADDR_IN 크기
    int addrLen = sizeof(clientService);
    //clientService를 0으로 밀기
    memset(&clientService, 0, sizeof(clientService));

    //클라이언트 접속시
    //accept(소켓, 클라주소 담은 구조체, 구조체 크기)
    //accept(sock,NULL,NULL);//클라이언트가 접속 할때까지 대기

    //클라이언트 접속시 클라이언트와 연락할 소켓을 반환
    //정보를 받을 SOCKADDR_IN을 넣어주면
    SOCKET acceptSocket = accept(listentSocket, (SOCKADDR*)&clientService, &addrLen);

    if (acceptSocket == INVALID_SOCKET)
    {
        printf("listen accept error : %d\n", WSAGetLastError());

        closesocket(listentSocket);
        //winsock DLL 종료
        WSACleanup();

        return 1;
    }
    printf("Client Connected!!\n");

    //프로그램 종료 하지 않게
    while (true)
    {
        //IP 주소를 덤을 char 배열
        char inAddress[16];

        //IPv4와 IPv6 주소를 binary 형태에서 사람이 알아보기 쉬은 텍스로 변환
        inet_ntop(AF_INET, &clientService.sin_addr, inAddress, sizeof(inAddress));
        //접속한 클라이언트의 IP주소 출력
        printf("client connected IP : %s\n", inAddress);

        char sendBuffer[] = "hello this is server";

        //send(클라랑 연결된 소캣, 보낼버퍼,크기,0(flags));
        if (send(acceptSocket, sendBuffer, sizeof(sendBuffer), 0) == SOCKET_ERROR)
        {
            //에러 코드 검사
            printf("Send Error : %d\n : ", WSAGetLastError());
            //클라이언트와 연결된 소켓 닫기
            closesocket(acceptSocket);
            //처음으로 다시
            break;
        }


        printf("send Date : %s\n", sendBuffer);

        char recvBuffer[512];

        //받을때 까지 대기 : 블럭형 함수
        //recv(연결된 소켓, 담을 버퍼, 버퍼의 크기,0(flags))
        //담을 공간에다가 데이터를 쓰고
        //얼마나 썻는지 반환 --> recvLen : 받을 데이터의 크기
        int recvLen = recv(acceptSocket, recvBuffer, sizeof(recvBuffer), 0);

        if (recvLen <= 0)
        {
            printf("recvBuffer is Error : %d\n", WSAGetLastError());

            break;
        }

        printf("recv buffer data : %s\n", recvBuffer);

        printf("recv buffer Length : %d\n", recvLen);

    }

    closesocket(listentSocket);
    //winsock DLL 종료
    WSACleanup();
}

