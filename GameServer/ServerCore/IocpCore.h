#pragma once
class IocpCore
{
private:
    HANDLE iocpHandle = nullptr;
public:
    IocpCore();
    ~IocpCore();
public:
    HANDLE GetHandle() const { return iocpHandle; }
public:
    //    void Register(HANDLE socket, ULONG_PTR key);
    //HANDLE socket, ULONG_PTR key -> IocpObj* iocpObj
    void Register(class IocpObj* iocpObj);
    bool ObserveIO(DWORD time = INFINITE);
    //INFINITE : 정수 , INFINITY : 실수
};

