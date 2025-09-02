#pragma once
#include "IocpObj.h"

//#include "CorePch.h"

class AcceptEvent;

class Listener : public IocpObj
{
private:
	SOCKET socket = INVALID_SOCKET;

public:

public:
	Listener() = default;
	~Listener();
public:
    //포인터로 전환
	bool StartAccept(class Service* service);
    //Accept함수에 등록해주는 용도
    void RegisterAccept(AcceptEvent* acceptEvnet);
    void ProcessAccept(AcceptEvent* acceptEvnet);
	void CloseSocket();

public:
    // IocpObj을(를) 통해 상속됨
    HANDLE GetHandle() override { return (HANDLE)socket; }
    void ObserveIO(IocpEvent* iocpEvnet, DWORD byteTransferred) override;
};

