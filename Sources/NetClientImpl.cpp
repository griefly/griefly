#include "NetClientImpl.h"
#include "MagicStrings.h"
#include "Mob.h"
#include "ItemFabric.h"

#include <assert.h>

#include <SDL_net.h>

INetClient* net_client = nullptr;

void NetClient::Init()
{
    auto val = new NetClient;
    val->connected_ = false;
    val->fail_ = false;
    val->amount_ticks_ = 0;
    val->hash_ = 0;
    val->number_last_message_ = 0;
    net_client = val;
}

INetClient* NetClient::GetNetClient()
{
    return net_client;
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

    GetItemFabric()->loadMap(convertor, true, data_.who);
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

    if (msg->text == Net::MAP_REQUEST)
    {
        std::stringstream raw_map;
        GetItemFabric()->saveMap(raw_map, true);

        Message map_message;
        map_message.from = number_last_message_;
        map_message.to = msg->to;
        map_message.type = Net::MAP_TYPE;

        map_message.text = raw_map.str();

        SendSocketMessage(*main_socket_, map_message);
        return Recv(msg);
    } 

    number_last_message_ = msg->message_number;
    if (msg->text == Net::MAKE_NEW)
    {
        msg->to = GetManager()->GetCreator();
    }
    else
    {
        msg->to = GetItemFabric()->GetPlayerId(msg->to);
    }
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
            //
        } 
        else if (message.text == Net::MAKE_NEW)
        {
            SYSTEM_STREAM << "New mob must created!" << std::endl;
            //message.to = man_->GetCreator();
        }
        else if (message.type != Net::ORDINARY_TYPE)
        {
            SYSTEM_STREAM << "Some system message taken with no need?" << std::endl;
            SYSTEM_STREAM << "message.message_number: " << message.message_number << std::endl
                            << "message.from: " << message.from << std::endl
                            << "message.to: " << message.to << std::endl
                            << "message.type: " << message.type << std::endl
                            << "message.text: " << message.text << std::endl; 
            continue;
        }

        messages_.push(message);
        ++counter;
    }
    return true;
}

unsigned int NetClient::Hash() const
{
    return hash_;
}