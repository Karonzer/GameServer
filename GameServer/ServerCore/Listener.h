#pragma once


//#include "CorePch.h"
class Listener
{
private:
	SOCKET socket = INVALID_SOCKET;

public:

public:
	Listener() = default;
	~Listener();
public :

public:
    //포인터로 전환
	bool StartAccept(class Service* service);
	void CloseSocket();
};

