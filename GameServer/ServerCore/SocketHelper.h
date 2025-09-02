#pragma once
class SocketHelper
{
public:
    //비동기 함수 포인터들
    static LPFN_ACCEPTEX AcceptEx;

public:
    static bool StartUp();
    static bool CleanUp();
public:
    static bool SetIoControl(SOCKET socket, GUID guid, LPVOID* func);
    static SOCKET CreateSocket();
    static void CloseSocket(SOCKET& socket);
public:
    //소켓 옵션들
    static bool SetReuseAddress(SOCKET socket, bool enable);
    static bool SetLinger(SOCKET socket, u_short onOff, u_short time);
public:
    //서버 쪽에서 필요한거
    static bool Bind(SOCKET socket, SOCKADDR_IN sockAddr);
    static bool Listen(SOCKET socket, int baclog = SOMAXCONN);

};

template<typename T>
static inline bool SetSocketOpt(SOCKET socket, int level, int optName, T optVal)
{
    return setsockopt(socket, level, optName, (char*)&optVal, sizeof(T)) != SOCKET_ERROR;
}


