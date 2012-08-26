/*#include "NetServerImpl.h"
#include "MapClass.h"

typedef Poco::ScopedLock<Poco::Mutex> Lock;

NetServer* NetServer::Init(Manager* man)
{
    NetServer* retval = new NetServer;
    if (retval == nullptr)
        return nullptr;

    retval->fail_ = false;
    retval->man_ = man;
    retval->port_ = 0;
    retval->started_ = false; 
    return retval;
}

NetServer::KvServerConnection::KvServerConnection(const Poco::Net::StreamSocket& s)
                                                 : TCPServerConnection(s) {}

void NetServer::KvServerConnection::run()
{
    // TODO: do all stuff
}

bool NetServer::Start(unsigned int port)
{
    if (started_ == true)
        return !(fail_ = true);
    port_ = port;
    try
    {
        main_socket_ = new Poco::Net::ServerSocket(port);
        factory_impl_ = new FactoryTCPConnection;
        tcp_server_ = new Poco::Net::TCPServer(factory_impl_, main_socket_);
        tcp_server_->start();
    }
    catch (Poco::Exception& e)
    {
        SYSTEM_STREAM << "Fail start server, port " << port << std::endl;
        SYSTEM_STREAM << "Error: " << e.displayText() << std::endl;
        delete tcp_server_;
        delete main_socket_;
        delete factory_impl_;
        fail_ = true;
        return false;
    }
    started_ = true;
    return true;
}

bool NetServer::Stop()
{
    if (started_ == false)
        return !(fail_ = true);

    try
    {
        tcp_server_->stop();
    }
    catch (Poco::Exception& e)
    {
        SYSTEM_STREAM << "Fail stop server" << std::endl; // Dunno
        SYSTEM_STREAM << "Error: " << e.displayText() << std::endl;
        fail_ = true;
        return false;
    }

    delete tcp_server_;
    delete main_socket_;
    delete factory_impl_;

    started_ = false;
    return true;
}

bool NetServer::Restart()
{
    return Stop() && Start(port_);
}

bool NetServer::IsFail()
{
    return fail_;
}

bool NetServer::Broadcast(const Message& msg)
{
    bool retval = true;
    Lock l(clients_access_);
    for (auto it = clients_.begin(); it != clients_.end(); ++it)
        if (SendSocketMessage(it->socket, msg) == false)
        {
            retval = false;
            SYSTEM_STREAM << "Fail send broadcast message to " << it->name << std::endl;
        };
    return retval;
}

bool NetServer::SendMap(Client* client)
{
    std::stringstream raw_map;
    IMainItem::fabric->saveMap(raw_map);
    Message msg_map;
    msg_map.text = raw_map.str();

}*/