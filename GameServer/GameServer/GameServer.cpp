#pragma once
#include "pch.h"
#include <Service.h>
#include <Listener.h>


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



    Service service(L"127.0.0.1", 5500);


    Listener listener;
    thread t(AcceptThread, listener.GetHandle());
    listener.StartAccept(service);

    t.join();

    return 0;
}