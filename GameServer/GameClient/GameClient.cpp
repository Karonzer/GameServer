
#include <iostream>
using namespace std;
#pragma comment(lib, "ws2_32.lib") //라이브러리 블러오기
#include <winsock2.h> //소켓 서버 만들기 위해 필요
#include<WS2tcpip.h>
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

    printf("connect to service");

    while (true)
    {
        //받을 버퍼 크기 할당 : 얼마 받을지 모르니까 우선 넉넉하게
        char recvBuffer[512];

        //받을때 까지 대기 : 블럭형 함수
        //recv(연결된 소켓, 담을 버퍼, 버퍼의 크기,0(flags))
        //담을 공간에다가 데이터를 쓰고
        //얼마나 썻는지 반환 --> recvLen : 받을 데이터의 크기
        int recvLen = recv(connectSocket, recvBuffer, sizeof(recvBuffer), 0);

        if (recvLen <= 0)
        {
            printf("recvBuffer is Error : %d\n", WSAGetLastError());

            break;
        }

        printf("recv buffer data : %s\n", recvBuffer);

        printf("recv buffer Length : %d\n", recvLen);

        char sendBuffer[] = "hello this is Client";

        //send(클라랑 연결된 소캣, 보낼버퍼,크기,0(flags));
        if (send(connectSocket, sendBuffer, sizeof(sendBuffer), 0) == SOCKET_ERROR)
        {
            //에러 코드 검사
            printf("Send Error : %d\n : ", WSAGetLastError());
            //클라이언트와 연결된 소켓 닫기
            closesocket(connectSocket);
            //처음으로 다시
            break;
        }

        Sleep(1000);
        //엔터를 첬을때 경우 연결 종료
        if (GetAsyncKeyState(VK_RETURN))
        {
            //연결 끊기
            //SD_RECEIVE(0) recv를 막는다 . 수신을 막는다
            //SD_SEND(1) send를 막는다, 송신을 막는다
            //SD_BOTH 둘다 막는다, 송수신믈 막는다
            shutdown(connectSocket, SD_BOTH);
        }


    }
    closesocket(connectSocket);
    WSACleanup();

    return 1;
}

