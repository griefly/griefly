#pragma once

#include <vector>

#include "NetInterface.h"

#include "SyncQueue.h"

#include <Poco/Net/TCPServer.h>
#include <Poco/Runnable.h>
#include <Poco/Net/SocketStream.h>
#include <Poco/Net/TCPServerConnection.h>

class NetServer: public INetServer
{
    friend class NetSender;
public:
    static NetServer* Init(Manager* man);

    virtual bool SendMap(Client* client) override;

    virtual bool Start(unsigned int port) override;
    virtual bool Restart() override;
    virtual bool Stop() override;

    virtual bool Broadcast(const Message& msg) override;

    virtual bool IsFail() override;
private:
    class NetSender: public Poco::Runnable
    {
    public:
        virtual void run() override;
    };

    // It recv all messages from client
    class KvServerConnection: public Poco::Net::TCPServerConnection
    {
    public:
        KvServerConnection(const Poco::Net::StreamSocket& s);
        virtual void run();
    private:
        SynchronizedQueue<Message> sendqueue_;
    };

    unsigned int port_;
    bool fail_;
    bool started_;

    Poco::Net::ServerSocket* main_socket_;
    typedef Poco::Net::TCPServerConnectionFactoryImpl<KvServerConnection> FactoryTCPConnection;
    FactoryTCPConnection* factory_impl_;
    Poco::Net::TCPServer* tcp_server_;

    Poco::Mutex clients_access_;
    std::vector<Client> clients_;

    Manager* man_;
};