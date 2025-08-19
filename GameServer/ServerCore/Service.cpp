#include "pch.h"
#include "Service.h"

Service::Service(wstring _ip, uint16 _port)
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("WSAStarup filed with error\n");
        return;
    }




    ////���� : ���� �ּ�
    ////sockaddr_in ����ü ����
    ////SOCKADDR_IN service  = {}; 0���� �ʱ�ȭ �ص���
    //SOCKADDR_IN service;
    ////memset(�����ּ�,��,ũ��);
    ////0 ���� �ش� ����ü(service)�ʱ�ȭ
    //memset(&service, 0, sizeof(service));
    ////address family : IPv4
    //service.sin_family = AF_INET;
    //inet_pton(AF_INET, "127.0.0.1", &service.sin_addr);
    //service.sin_port = htons(5500);

    sockAddr.sin_family = AF_INET;
    IN_ADDR address;
    InetPton(AF_INET, _ip.c_str(), &address);
    sockAddr.sin_addr = address;
    sockAddr.sin_port = htons(_port);



}

Service::~Service()
{
    WSACleanup();
}
