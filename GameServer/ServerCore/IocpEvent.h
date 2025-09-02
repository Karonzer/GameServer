#pragma once

enum class EventType : uint8
{
    CONNECT,
    DISCONNECT,
    ACCEPT,
    RECV,
    SEND,
};

class IocpEvent : public OVERLAPPED
{
public:
    EventType eventType;
    //나를 들고 있는 주체가 누구인지
    class IocpObj* owner;
public:
    IocpEvent(EventType type);
public:
    void Init();
};

class AcceptEvent : public IocpEvent
{
public:
    class Session* session = nullptr;



public:
    //생성장 호출 될때 부모에 있는 생성자 IocpEvent(EventType tpye)호출
    //매개변수로 나의 타입은 ACCEPT이라고 넣어줌
    AcceptEvent() : IocpEvent(EventType::ACCEPT) {}
};

