#pragma once
//#include "CorePch.h"

class IocpCore;
class Listener;

class Service
{
private:
	SOCKADDR_IN sockAddr = {};
    Listener* listener = nullptr;
    IocpCore* iocpCore = nullptr;
public:
    Service(wstring _ip, uint16 _port);
    ~Service();
public:
	SOCKADDR_IN& GetSockAddr(){ return sockAddr; }
    IocpCore* GetIocpCore() const { return iocpCore; }

public:
    bool Start();


};

