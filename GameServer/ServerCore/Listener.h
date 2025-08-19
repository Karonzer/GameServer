#pragma once
#include "CorePch.h"
class Listener
{
private:
	SOCKET socket = INVALID_SOCKET;
	HANDLE iocpHandle = nullptr;
public:

public:
	Listener();
	~Listener();
public :
	HANDLE GetHandle() const { return iocpHandle; }
public:
	bool StartAccept(class Service& service);
	void CloseSocket();
};

