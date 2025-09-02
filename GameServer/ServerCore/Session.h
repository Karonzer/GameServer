#pragma once
class Session
{
public:
    //WSAOVERLAPPED overlapped = {};
private:
    SOCKET socket = INVALID_SOCKET;
public:
    BYTE recvBuffer[1024] = {};

public:
    Session();
    ~Session();
public:
    SOCKET GetSocket() const { return socket; }
};

