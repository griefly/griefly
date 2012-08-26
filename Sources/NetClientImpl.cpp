#include "NetClientImpl.h"
#include "MagicStrings.h"

#include <assert.h>

typedef Poco::ScopedLock<Poco::Mutex> MutexLock;

NetClient* NetClient::Init(Manager* man)
{
    if (man == nullptr)
        return nullptr;
    auto retval = new NetClient;
    retval->man_ = man;
    retval->connected_ = false;
    retval->fail_ = false;
    retval->amount_ticks_ = 0;
    retval->hash_ = 0;
    return retval;
}

bool NetClient::Connect(const std::string& ip, unsigned int port, LoginData data)
{
    if (connected_ == true)
        return !(fail_ = true);

    ip_ = ip;
    port_ = port;

    main_socket_ = nullptr;
    recv_client_ = nullptr;
    task_ = nullptr;

    try
    {
        Poco::Net::SocketAddress connect_address(ip, port);

        main_socket_ = new Poco::Net::StreamSocket(connect_address);
        task_ = new NetClient::RecvFromServer(this);

        Message message(NET_LOGIN_DATA);
        message.from = data.who;
        message.to = data.word_for_who;

        // Login
        if (SendSocketMessage(*main_socket_, message) == false)
        {
            SYSTEM_STREAM << "Fail send login information" << std::endl;
            delete main_socket_;
            delete task_;
            return !(fail_ = true);
        }
        if (RecvSocketMessage(*main_socket_, &message) == false)
        {
            SYSTEM_STREAM << "Fail receive login information" << std::endl;
            delete main_socket_;
            delete task_;
            return !(fail_ = true);
        }

        data_.who = message.from;
        data_.word_for_who = message.to;

        // Map
        if (RecvSocketMessage(*main_socket_, &message) == false)
        {
            SYSTEM_STREAM << "Fail receive map information" << std::endl;
            delete main_socket_;
            delete task_;
            return !(fail_ = true);
        }

        std::stringstream convertor;
        convertor << message.text;

        // TODO: do mutex lock
        IMainItem::fabric->loadMap(convertor);
        convertor.str("");

        recv_client_ = new Poco::Thread;
        recv_client_->start(*task_);

        connected_ = true;
        return true;
    }
    catch (Poco::Exception& e)
    {
        SYSTEM_STREAM << e.displayText() << std::endl;
        delete main_socket_;
        delete recv_client_;
        delete task_;
        return !(fail_ = true);
    }
}

bool NetClient::Disconnect()
{
    if (connected_ == false)
        return !(fail_ = true);

    delete recv_client_;
    
    bool retval = true;

    try
    {
        main_socket_->close();
    }
    catch (Poco::Exception& e)
    {
        SYSTEM_STREAM << e.displayText() << std::endl;
        retval = !(fail_ = true);
    }

    delete recv_client_;
    delete task_;

    fail_ = false;
    return retval;
}

bool NetClient::Reconnect()
{
    Disconnect();
    return Connect(ip_, port_, data_);
}

bool NetClient::IsFail()
{
    return fail_;
}

bool NetClient::Send(const Message& msg)
{
    if (SendSocketMessage(*main_socket_, msg) == false)
    {
        fail_ = true;
        return false;
    }
    return true;
}

bool NetClient::Ready()
{
    MutexLock l(lock_amount_ticks_);
    if (amount_ticks_ == 0)
        return false;
    --amount_ticks_;
    return true;
}

bool NetClient::Recv(Message* msg)
{
    assert(msg && "Try fill nullptr");
    *msg = messages_.Get();
    return true;
}

NetClient::RecvFromServer::RecvFromServer(NetClient* client)
    : client_(client) {}

void NetClient::RecvFromServer::run()
{
    SYSTEM_STREAM << "Start recv from server\n";

    while (true)
    {
        Message message;
        if(RecvSocketMessage(*client_->main_socket_, &message) == false)
            return; // Fail

        if (message.text == NET_NEXTTICK)
        {
            MutexLock l(client_->lock_amount_ticks_);
            ++client_->amount_ticks_;
        }
        else if (message.text == NET_HASH)
        {
            client_->hash_ = message.from;
            continue;
        }

        client_->messages_.Put(message);     
    }

    SYSTEM_STREAM << "Fail recive socket message, recive thread ended his work" << std::endl;
}