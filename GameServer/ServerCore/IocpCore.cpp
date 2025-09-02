#include "pch.h"
#include "IocpCore.h"
//#include "Session.h"
#include "IocpEvent.h"
#include "IocpObj.h"
IocpCore::IocpCore()
{
    iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);
}

IocpCore::~IocpCore()
{
    CloseHandle(iocpHandle);
}

void IocpCore::Register(IocpObj* iocpObj)
{
    //iocpObj->GetHandle : return (HANDLE)socket
    //key는 안 사용 할꺼라 0
    CreateIoCompletionPort(iocpObj->GetHandle(), iocpHandle, 0, 0);
}

bool IocpCore::ObserveIO(DWORD time)
{
    DWORD bytesTransferred = 0;
    ULONG_PTR key = 0;
    //WSAOVERLAPPED overlapped = {};

    //Session iocpEvnet로 교체
    //Session* session = nullptr;
    IocpEvent* iocpEvent = nullptr;

    printf("GameServer watting..\n");


    if (GetQueuedCompletionStatus(iocpHandle, &bytesTransferred, &key, (LPOVERLAPPED*)&iocpEvent, INFINITE))
    {
        //Session 하고 Listnener는 IocpObj를 상속받을 꺼임
        IocpObj* icopobj = iocpEvent->owner;
        //IocpObj의 ObserveIO는 가상함수이기 때문에
        //1. IocpObj가 할당된 아이가 Session이라면 Session->ObserveIO를 실행
        //2. IocpObj가 할당된 아이가 Listnener이라면 Listnener->ObserveIO를 실행
        icopobj->ObserveIO(iocpEvent, bytesTransferred);


        //printf("Event occur\n"); // 연결 성공 메세지 출력
        //switch (iocpEvent->eventType)
        //{
        //case EventType::ACCEPT:
        //    printf("Client ACCEPT\n");
        //    break;
        //default:
        //    break;
        //}
    }
    else
    {

        //printf("GetQueuedCompletionStatus function errer\n");
        switch (GetLastError())
        {
        case WAIT_TIMEOUT:
            printf("GetQueuedCompletionStatus WAIT_TIMEOUT errer\n");
            //GetQueuedCompletionStatus 기다리는 시간을 넘아간거니까
            return false;
        default:
            break;
        }
        return false;
    }
    return true;
}
