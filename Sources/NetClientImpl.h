#pragma once

#include "NetInterface.h"

#include "SyncQueue.h"

#include <Poco/Net/TCPServer.h>
#include <Poco/Runnable.h>
#include <Poco/Net/SocketStream.h>

#include <memory>

class NetClient: public INetClient
{
public:
    static NetClient* Init(Manager* man);

    virtual bool Connect(const std::string& ip, unsigned int port, LoginData data) override;
    virtual bool Disconnect() override;
    virtual bool Reconnect() override;

    virtual bool Ready() override;
    virtual bool Recv(Message* msg) override;
    virtual bool Send(const Message& msg) override;
    virtual bool IsFail() override;
private:
    friend class RecvFromServer;
    class RecvFromServer: public Poco::Runnable
    {
    public:
        RecvFromServer(NetClient* client);
        virtual void run() override;
    private:
        NetClient* client_;
    };

    std::string ip_;
    unsigned int port_;
    LoginData data_;

    RecvFromServer* task_;
    Poco::Thread* recv_client_;
    Poco::Net::StreamSocket* main_socket_;

    SynchronizedQueue<Message> messages_;

    unsigned int amount_ticks_;
    Poco::Mutex lock_amount_ticks_;

    bool connected_;
    bool fail_;
    bool ready_;

    unsigned int hash_;

    Manager* man_;
};