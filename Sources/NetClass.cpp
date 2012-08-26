#include <assert.h>

#include "NetClass.h"
#include "ServerConnection.h"

#include "MagicStrings.h"

#include <Poco/Net/NetException.h>

#include "constheader.h"

#include "MapClass.h"

typedef Poco::ScopedLock<Poco::Mutex> Lock;

NetMain* NetMaster = nullptr;

NetMain::NetMain()
{
    can_send = true;
    is_empty = false;
}

bool NetMain::CanSend()
{
    Lock lock(new_connection);
    return can_send;
}

void NetMain::ForbidSend()
{
    Lock lock(new_connection);
    assert(can_send);
    SendToSockets(std::string("0 ") + NET_SIGNAL);
    can_send = false;
    while(!is_empty)
        is_empty_c.wait(new_connection);
}

void NetMain::SignalEndProcess()
{
    Lock lock(new_connection);
    assert(!is_empty);
    is_empty = true;
    is_empty_c.signal();
}

void NetMain::PermitSend()
{
    Lock lock(new_connection);
    can_send = true;
    is_empty = false;
}

void NetMain::SendToSockets(const std::string& str)
{
    while (!CanSend());
    Lock lock(qmut);
    for (auto it = queues.begin(); it != queues.end(); ++it)
        (*it)->Put(str);
}

void NetMain::StartServer(int port)
{
    try
    {
        SYSTEM_STREAM << "Start server...\n";
        sock = new Poco::Net::ServerSocket(port);
        srv = new Poco::Net::TCPServer(new Poco::Net::TCPServerConnectionFactoryImpl<KvServerConnection>, *sock);
        srv->start();
        SYSTEM_STREAM << "Sucess!\n";
    }
    catch (Poco::Exception& exc)
	{
		SYSTEM_STREAM << "void NetMain::StartServer(int port) StartServer: " << exc.displayText() << std::endl;
        return;
	}
}

void NetMain::StartClient(int port, std::string adr)
{
    try
    {
        Poco::Net::SocketAddress sa(adr, port);
        sockstr = new Poco::Net::StreamSocket(sa);
        recv_client = new Poco::Thread;
        task = new RecvFromServer;
        task->sockstr = sockstr;
        recv_client->start(*task);
    }
    catch(Poco::Exception& e)
    {
        SYSTEM_STREAM << e.displayText() << "\nvoid NetMain::StartClient(int port, std::string adr)\n";
        return;
    }
}

NetMain::~NetMain()
{
    delete sock;
    delete srv;
    delete sockstr;
    delete recv_client;
}

void NetMain::SendTick()
{
    try
    {
        SendToSockets(std::string("0 ") + NET_NEXTTICK);
    }
    catch(Poco::Exception& e)
    {
        SYSTEM_STREAM << e.displayText() << "\nvoid NetMain::SendTick()\n";
        return;
    }
}

void RecvFromServer::run()
{
    SYSTEM_STREAM << "Start recv from server\n";
    std::stringstream ss;
    std::string loc1, loc2;
    try
    {
        if(IMainItem::mobMaster->GetMode() == CLIENT)
        {
            IMainItem::mobMaster->map_access.lock();
            IMainItem::fabric->loadMapFromNet();
            IMainItem::mobMaster->map_access.unlock();
        }
        while(!ss.fail())
        {
            loc1.clear();
            loc2.clear();
            ss.clear();
            NetMain::RecvFromStrSock(*sockstr, ss);
            ss >> loc1;
            ss >> loc2;
            if (loc2 == NET_SIGNAL)
            {
                if (IMainItem::mobMaster->GetMode() != SERVER)
                    continue;
                NetMaster->new_connection.lock();
                NetMaster->is_empty = true;
                NetMaster->is_empty_c.signal();
                NetMaster->new_connection.unlock();
                //SYSTEM_STREAM << "NetMaster->new_connection.unlock(); in void RecvFromServer::run()\n";
                continue;
            }
            if (loc2 == NET_NEXTTICK)
            {
                //SYSTEM_STREAM << NET_NEXTTICK << "\n";
                IMainItem::mobMaster->tick_recvm.lock();
                ++IMainItem::mobMaster->tick_recv;
                IMainItem::mobMaster->tick_recvm.unlock();
            }
            if (loc2 == "hash")
            {
                std::stringstream loc;
                loc << loc1;
                unsigned int num;
                loc >> num;
                NetMaster->hash = num;
                continue;
            }
            //SYSTEM_STREAM << loc1 + " " + loc2 << " RECV MESSAGE\n";
            NetMaster->in_messages.Put(loc1 + " " + loc2);
            if (loc2 == NET_END)
                break;
        }
        SYSTEM_STREAM << "END RECV FROM SERVER\n";
    }
    catch(Poco::Exception& e)
    {
        SYSTEM_STREAM << e.displayText() << "\nvoid RecvFromServer::run()\n";
        return;
    }
}

void NetMain::RecvFromStrSock(Poco::Net::StreamSocket& sockstr, std::stringstream& ss)
{
    char buff[BUFF_SIZE + 1];
    buff[BUFF_SIZE] = '\0';
    int result = 0;
    int oldresult = result;
    while((result = sockstr.receiveBytes(buff + result, BUFF_SIZE - oldresult)) != BUFF_SIZE - oldresult)
        oldresult = result;
    ss << buff;
}

void NetMain::SendToStrSock(Poco::Net::StreamSocket& sockstr, std::stringstream& ss)
{
    int result = 0;
    int oldresult = result;
    for(int i = (BUFF_SIZE - (int)ss.str().size()); i > 0; --i)
        ss << " ";
    if(ss.str().size() != 32)
        SYSTEM_STREAM << "\n" << ss.str().size() << "\n"; 
    while((result = sockstr.sendBytes(ss.str().c_str() + result, (int)ss.str().size() - oldresult)) != (int)ss.str().size() - oldresult)
        oldresult = result;
}

size_t NetMain::NewMob()
{
    std::stringstream loc;
    loc << now_creator;
    for(auto it = queues.begin(); it != queues.end(); ++it)
        (*it)->Put(loc.str() + " NEWMOB");
    SYSTEM_STREAM << NetMain::now_creator.ret_id() << " now creator\n";
    return NetMain::now_creator->WaitCreateItem();
}

void NetMain::SendRand()
{
    try
    {
        int r = rand();
        std::stringstream loc;
        loc << r;
        SendToSockets(loc.str() + " rand");
    }
    catch(Poco::Exception& e)
    {
        SYSTEM_STREAM << e.displayText() << "\nvoid NetMain::Sendget_rand()\n";
        return;
    }
}

void NetMain::SendHash()
{
    try
    {
        unsigned int r = IMainItem::fabric->get_hash_last();
        std::stringstream loc;
        loc << r;
        SendToSockets(loc.str() + " hash");
    }
    catch(Poco::Exception& e)
    {
        SYSTEM_STREAM << e.displayText() << "\nvoid NetMain::SendHash()\n";
        return;
    }
}