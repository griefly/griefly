#pragma once

#include "Message.h"

const unsigned int DEFAULT_PORT = 1111;
const char* const DEFAULT_IP = "127.0.0.1";

struct LoginData
{
    LoginData()
        : who(0),
        word_for_who(0),
        jid("default@default.default") {}
    size_t who;
    unsigned int word_for_who;
    std::string jid;
};

class Manager;

class INetClient
{
public:
    virtual bool Connect(const std::string& ip, unsigned int port, LoginData data) = 0;
    virtual bool Reconnect() = 0;
    virtual bool Disconnect() = 0;

    virtual bool Process() = 0;

    virtual bool Ready() = 0;
    virtual bool Recv(Message* msg) = 0;
    virtual bool Send(const Message& msg) = 0;
    virtual bool IsFail() = 0;

    virtual unsigned int Hash() const = 0;
protected:
    INetClient() {}
};