#include "pch.h"
#include "Service.h"
#include "SocketHelper.h"
#include "IocpCore.h"
#include "Listener.h"

Service::Service(wstring _ip, uint16 _port)
{
    SocketHelper::StartUp();

    ////정보 : 서버 주소
    ////sockaddr_in 구조체 연동
    ////SOCKADDR_IN service  = {}; 0으로 초기화 해도됨
    //SOCKADDR_IN service;
    ////memset(시작주소,값,크기);
    ////0 으로 해당 구조체(service)초기화
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

    iocpCore = new IocpCore;

}

Service::~Service()
{
    if (iocpCore != nullptr)
    {
        delete iocpCore;
        iocpCore = nullptr;
    }

    if (listener != nullptr)
    {
        delete listener;
        listener = nullptr;
    }
    WSACleanup();
}

bool Service::Start()
{
    listener = new Listener;
    return listener->StartAccept(this);
}
