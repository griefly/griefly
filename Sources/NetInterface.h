#pragma once

#include "Mob.h"
#include "Message.h"

//const unsigned int DEFAULT_PORT = 2398;
//const char* const DEFAULT_IP = "127.0.0.1";

struct LoginData
{
    LoginData()
        : who(0),
        word_for_who(0) {}
    size_t who;
    unsigned int word_for_who;
};

class INetClient
{
public:
    virtual bool Connect(const std::string& ip, unsigned int port, LoginData data) = 0;
    virtual bool Reconnect() = 0;
    virtual bool Disconnect() = 0;

    virtual bool Ready() = 0;
    virtual bool Recv(Message* msg) = 0;
    virtual bool Send(const Message& msg) = 0;
    virtual bool IsFail() = 0;
protected:
    INetClient() {}
};

struct Client
{
    LoginData data;
    std::string name;
    Poco::Net::StreamSocket& socket;

    // TODO: whatever
};

// TODO: not virtual?
class INetServer
{
public:
    virtual bool SendMap(Client* client) = 0;

    virtual bool Start(unsigned int port) = 0;
    virtual bool Restart() = 0;
    virtual bool Stop() = 0;

    virtual bool Broadcast(const Message& msg) = 0;

    virtual bool IsFail() = 0;
};