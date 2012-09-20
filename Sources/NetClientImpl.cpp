#include "NetClientImpl.h"
#include "MagicStrings.h"
#include "Mob.h"
#include "ItemFabric.h"

#include <assert.h>

#include <SDL_net.h>

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
    retval->number_last_message_ = 0;
    return retval;
}

bool NetClient::Connect(const std::string& ip, unsigned int port, LoginData data)
{
    if (connected_ == true)
        return !(fail_ = true);
    
    ip_ = ip;
    port_ = port;

    main_socket_ = nullptr;

    IPaddress ip_local;
    if (SDLNet_ResolveHost(&ip_local, ip.c_str(), port) == -1)
    {
        SYSTEM_STREAM << SDLNet_GetError() << std::endl;
        return !(fail_ = true);
    }

    main_socket_ = new TCPsocket;
    *main_socket_ = SDLNet_TCP_Open(&ip_local);
    if (!*main_socket_)
    {
        SYSTEM_STREAM << SDLNet_GetError() << std::endl;
        delete main_socket_;
        return !(fail_ = true);
    }
    Message message;
    message.from = data.who;
    message.to = data.word_for_who; // TODO: take word from net
    message.text = data.jid;

    // Login
    if (SendSocketMessage(*main_socket_, message) == false)
    {
        SYSTEM_STREAM << "Fail send login information" << std::endl;
        delete main_socket_;
        return !(fail_ = true);
    }
    if (RecvSocketMessage(*main_socket_, &message) == false)
    {
        SYSTEM_STREAM << "Fail receive login information" << std::endl;
        delete main_socket_;
        return !(fail_ = true);
    }

    data_.who = message.to;
    data_.word_for_who = message.from;
    data_.jid = data.jid;

    if (message.text == Net::NOMAP)
    {
        connected_ = true;
        return true;
    }

    if (RecvSocketMessage(*main_socket_, &message) == false)
    {
        SYSTEM_STREAM << "Fail receive map information" << std::endl;
        delete main_socket_;
        return !(fail_ = true);
    }

    std::stringstream convertor;
    convertor << message.text;

    // TODO: change owner

    IMainItem::fabric->loadMap(convertor, data_.who);
    convertor.str("");

    connected_ = true;
    return true;
}

bool NetClient::Disconnect()
{
    if (connected_ == false)
        return !(fail_ = true);
    
    bool retval = true;

    SDLNet_TCP_Close(*main_socket_);
    
    delete main_socket_;

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
    if (amount_ticks_ == 0)
        return false;
    --amount_ticks_;
    return true;
}

bool NetClient::Recv(Message* msg)
{
    assert(msg && "Try fill nullptr");
    *msg = messages_.front();
    messages_.pop();
    number_last_message_ = msg->message_number;
    return true;
}


bool NetClient::Process()
{
    const unsigned int MAX_MSG = 128;
    unsigned int counter = 0;
    while (SocketReady(*main_socket_) && counter < MAX_MSG)
    {
        Message message;
        if(RecvSocketMessage(*main_socket_, &message) == false)
        {
            SYSTEM_STREAM << "Fail message receive" << std::endl;
            return false;
        } // Fail

        if (message.text == Net::NEXTTICK)
        {
            ++amount_ticks_;
        }
        else if (message.text == Net::HASH)
        {
            hash_ = message.from;
            continue;
        }
        else if (message.text == Net::MAP_REQUEST)
        {
            std::stringstream raw_map;
            IMainItem::fabric->saveMap(raw_map);

            Message map_message;
            map_message.from = number_last_message_;
            map_message.to = message.to;
            map_message.type = Net::MAP_TYPE;

            map_message.text = raw_map.str();

            SendSocketMessage(*main_socket_, map_message);
            continue;
        }
        else if (message.text == Net::MAKE_NEW)
        {
            SYSTEM_STREAM << "New mob must created!" << std::endl;
            message.to = man_->GetCreator();
        }
        

        messages_.push(message);
        ++counter;
    }
    return true;
}