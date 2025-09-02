#pragma once
#include "pch.h"
#include <Service.h>
#include <IocpCore.h>




int main()
{
    printf("-----------Server----------\n");

    Service* serverService = new Service(L"127.0.0.1", 5500);
    thread t
    (
        [=]()
        {
            while (true)
            {
                serverService->ObserveIO();
            }
        }
    );

    serverService->Start();

    t.join();

    delete serverService;


    return 0;
}