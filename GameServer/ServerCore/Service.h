#pragma once
#include "CorePch.h"
class Service
{
private:
	SOCKADDR_IN sockAddr = {};
public:
	SOCKADDR_IN GetSockAddr() const { return sockAddr; }

public:
	//Service(wstring ip, u_short port);
	Service(wstring _ip, uint16 _port);
	~Service();



};

