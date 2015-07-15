#include "NetClientImpl.h"

#include <assert.h>

#include <SDL_net.h>

#include "MagicStrings.h"
#include "Mob.h"
#include "ItemFabric.h"
#include "Creator.h"
#include "Debug.h"

INetClient* net_client = nullptr;

void NetClient::Init()
{
    auto val = new NetClient;
    val->connected_ = false;
    val->fail_ = false;
    val->amount_ticks_ = 0;
    val->hash_ = 0;
    val->number_last_message_ = 0;
    val->hash_tick_ = 0;
    val->last_ping_send_ = 0;
    val->current_ping_ = 999;
    net_client = val;
}

INetClient* NetClient::GetNetClient()
{
    return net_client;
}

unsigned int NetClient::Ping() const
{
    return current_ping_;
}

bool NetClient::SendProtocolVersion()
{
    // Version 1.0
    // "S13*"
    // * - version
    std::string sendval = "S131";

    const char* begin_pos = sendval.c_str();
    size_t length = sendval.length();

    if (SDLNet_TCP_Send(*main_socket_, begin_pos, length) != length)
    {
        SYSTEM_STREAM << SDLNet_GetError() << std::endl;
        return false;
    }
    return true;
}

#define XSTR(x) #x
#define STR(x) XSTR(x)

bool NetClient::SendAndCheckClientVersion()
{
    // It is compile time macro with version (/D or -D)
    std::string client_version = STR(DEFINED_VERSION);

    Message message;
    message.text = client_version;

    // Check version
    if (SendSocketMessage(*main_socket_, message) == false)
    {
        SYSTEM_STREAM << "Fail send verison information" << std::endl;
        return false;
    }
    if (RecvSocketMessage(*main_socket_, &message) == false)
    {
        SYSTEM_STREAM << "Fail receive verison information" << std::endl;
        return false;
    }

    if (message.text != Net::GOOD_VERSION)
    {
        SYSTEM_STREAM << "Wrong version" << std::endl;
        return false;
    }

    return true;
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

    if (!SendProtocolVersion())
    {
        SYSTEM_STREAM << SDLNet_GetError() << std::endl;
        SDLNet_TCP_Close(*main_socket_);
        delete main_socket_;
        return !(fail_ = true);
    }

    if (!SendAndCheckClientVersion())
    {
        SDLNet_TCP_Close(*main_socket_);
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
        SDLNet_TCP_Close(*main_socket_);
        delete main_socket_;
        return !(fail_ = true);
    }
    if (RecvSocketMessage(*main_socket_, &message) == false)
    {
        SYSTEM_STREAM << "Fail receive login information" << std::endl;
        SDLNet_TCP_Close(*main_socket_);
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
        SDLNet_TCP_Close(*main_socket_);
        delete main_socket_;
        return !(fail_ = true);
    }

    std::stringstream convertor;
    convertor << message.text;

    GetItemFabric()->loadMap(convertor, true, data_.who);
    convertor.str("");

    connected_ = true;
    amount_ticks_ = 0;
    return true;
}

bool NetClient::Disconnect()
{
    if (connected_ == false)
        return !(fail_ = true);
    
    bool retval = true;

    SDLNet_TCP_Close(*main_socket_);
    
    delete main_socket_;
    main_socket_ = nullptr;

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
    if (IsFail())
        return false;

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
    if (IsFail())
        return false;

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
        msg->to = GetCreator();
    }
    else
    {
        msg->to = GetItemFabric()->GetPlayerId(msg->to);
    }
    return true;
}


bool NetClient::Process()
{
    if (IsFail())
        return false;

    const unsigned int MAX_MSG = 128;
    unsigned int counter = 0;
    while (SocketReady(*main_socket_) && counter < MAX_MSG)
    {
        Message message;
        if (RecvSocketMessage(*main_socket_, &message) == false)
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
            hash_tick_ = message.to;

            Debug::UnsyncDebug().AddNetSyncPair(hash_, hash_tick_);
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
        else if (message.text == Net::PING_RESPONSE)
        {
            current_ping_ = SDL_GetTicks() - last_ping_send_;
            continue;
        }
        else if (   message.type != Net::ORDINARY_TYPE
                 && message.type != Net::CHAT_TYPE)
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

    const unsigned int PING_TIMER = 1000;
    if (SDL_GetTicks() - last_ping_send_ > PING_TIMER)
    {
        Message ping_message;
        ping_message.type = Net::SYSTEM_TYPE;
        ping_message.text = Net::PING_REQUEST;
        if (SendSocketMessage(*main_socket_, ping_message) == false)
        {
            SYSTEM_STREAM << "Fail send ping message" << std::endl;
            return false;
        }
        last_ping_send_ = SDL_GetTicks();
    }

    return true;
}

unsigned int NetClient::Hash() const
{
    return hash_;
}

unsigned int NetClient::HashTick() const
{
    return hash_tick_;
}
