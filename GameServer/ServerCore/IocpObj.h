#pragma once
class IocpObj
{
public:
    virtual HANDLE GetHandle() abstract;
    virtual void ObserveIO(class IocpEvent* iocpEvent, DWORD byteTransferred = 0) abstract;
};

