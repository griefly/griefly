#pragma once

#include <Poco/Net/TCPServer.h>
#include <Poco/Runnable.h>
#include <Poco/Net/SocketStream.h>

#include <string>
#include <strstream>
#include <vector>

#include "SyncQueue.h"
#include "ServerConnection.h"
#include "Teleportator.h"
#include "constheader.h"

const int BUFF_SIZE = 32;

class RecvFromServer;

class NetMain
{
public:
    NetMain();
    ~NetMain();
    std::vector<SynchronizedQueue<std::string>*> queues;//clients queue
    Poco::Mutex qmut;
    static void RecvFromStrSock(Poco::Net::StreamSocket& sockstr, std::stringstream& str);
    static void SendToStrSock(Poco::Net::StreamSocket& sockstr, std::stringstream& str);
    void SendToSockets(const std::string& str);
    size_t NewMob();//server stuff
    void StartServer(int port = DEFAULT_PORT);
    void StartClient(int port = DEFAULT_PORT, std::string host = DEFAULT_ADRS);
    void SendTick();
    void SendRand();
    void SendHash();
    id_ptr_on<Teleportator> now_creator;//TODO: make getCreator function
    SynchronizedQueue<std::string> in_messages;

    unsigned int hash;

    // New connection stuff
    Poco::Mutex only_one_connection;
    Poco::Mutex new_connection;
    bool can_send;
    bool is_empty;
    Poco::Condition is_empty_c;

    void SignalEndProcess(); // MapMaster call this, when it end process all and read fin message
    bool CanSend(); // Can we send something from server to clients?

    void ForbidSend(); // Forbid and send end tick plus special message, wait until special message dont recieve
    void PermitSend(); // Just permit send
    // End new connect stuff

    Poco::Net::ServerSocket* sock;
    Poco::Net::TCPServer* srv;

    RecvFromServer* task;
    Poco::Thread* recv_client;
    Poco::Net::StreamSocket* sockstr;//client
};

class RecvFromServer: public Poco::Runnable
{
public:
    Poco::Net::StreamSocket* sockstr;
    void virtual run();
};


extern NetMain* NetMaster; 