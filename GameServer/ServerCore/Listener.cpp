#include "pch.h"
#include "Listener.h"
#include "Service.h"
#include "SocketHelper.h"
#include "IocpCore.h"
#include "Session.h"
#include "IocpEvent.h"


Listener::~Listener()
{
    CloseSocket();
}

bool Listener::StartAccept(Service* service)
{
    socket = SocketHelper::CreateSocket();
    if (socket == INVALID_SOCKET)
        return false;

    if (!SocketHelper::SetReuseAddress(socket, true))
        return false;


    if (!SocketHelper::SetLinger(socket, 0, 0))
        return false;

    ULONG_PTR key = 0;
    service->GetIocpCore()->Register(this);


    if (!SocketHelper::Bind(socket, service->GetSockAddr()))
        return false;

    if (!SocketHelper::Listen(socket))
        return false;

    //acceptevnet 지금은 해제가 얘매함...
    AcceptEvent* acceptevnet = new AcceptEvent;
    acceptevnet->owner = this;
    RegisterAccept(acceptevnet);


    return true;
}

void Listener::RegisterAccept(AcceptEvent* acceptEvnet)
{

    //실패 할때마다 Session이 쌓일 예정이라 그냥 스마트 포인토 교체
    Session* session = new Session();
    acceptEvnet->Init();
    acceptEvnet->session = session;
    DWORD dwBytes = 0;

    if (!SocketHelper::AcceptEx(socket, session->GetSocket(), session->recvBuffer, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwBytes, (LPOVERLAPPED)acceptEvnet))
    {
        if (WSAGetLastError() != ERROR_IO_PENDING)
        {
            //PENDING이 아니라면 문제가 있는거니가 그건 버리고 새롭게
            //RegisterAccept을 실행 시킴
            RegisterAccept(acceptEvnet);
        }

    }
}

void Listener::ProcessAccept(AcceptEvent* acceptEvnet)
{
    printf("ProcessAccept\n");
}

void Listener::CloseSocket()
{
    SocketHelper::CloseSocket(socket);
}


//
void Listener::ObserveIO(IocpEvent* iocpEvnet, DWORD byteTransferred)
{
    ProcessAccept((AcceptEvent*)iocpEvnet);
}
