#include "ServerConnection.h"
#include "NetClass.h"
#include "MapClass.h"
#include "SyncQueue.h"

#include <vector>
#include <string>
#include <algorithm>

#include <Poco/Net/SocketStream.h>
#include <Poco/Thread.h>
#include <Poco/Runnable.h>
#include <Poco/Net/NetException.h>

class RecvConnection: public Poco::Runnable
{
public:
    KvServerConnection* connect;
    std::stringstream ss;
    Poco::Net::StreamSocket* strsock;
    id_ptr_on<IMob> ptr;
    virtual void run()
    {
        try
        {
            SYSTEM_STREAM << "\nNew recv server thread start\n";
            char id[10];
            _itoa_s(ptr.ret_id(), id, 10);
            while(!ss.fail())
            {
                NetMain::RecvFromStrSock(*strsock, ss);
                std::string loc;
                ss >> loc;
                if(loc == "end")
                    return;
                loc = " " + loc;
                NetMaster->SendToSockets(id + loc);
            }
            SYSTEM_STREAM << "\nNew recv server thread end\n";
        } 
        catch(Poco::Exception& e)
        {
            SYSTEM_STREAM << e.displayText() << " class RecvConnection: public Poco::Runnable \n";
            return;
        }
   }
};

void KvServerConnection::run()
{
    try
    {
        Poco::ScopedLockWithUnlock<Poco::Mutex> lock(NetMaster->only_one_connection);
        std::stringstream str;
        NetMaster->qmut.lock();
        id_ptr_on<IMob> newmob = IMainItem::mobMaster->thisMob;
        if(NetMaster->queues.size())
        {
            NetMaster->qmut.unlock();
            SYSTEM_STREAM << "IMainItem::mobMaster->map_access.lock();\n";
            newmob = NetMaster->NewMob();
            SYSTEM_STREAM << "id_ptr_on<IMob> newmob = NetMaster->NewMob();\n";
            NetMaster->ForbidSend();
            IMainItem::mobMaster->map_access.lock();
            str << MAIN_TICK << std::endl;
            str << IMainItem::fabric->get_id() << std::endl;
            str << newmob.ret_id() << std::endl;
            SYSTEM_STREAM << "\n" << MAIN_TICK << std::endl;
            SYSTEM_STREAM << IMainItem::fabric->get_id() << std::endl;
            SYSTEM_STREAM << newmob.ret_id() << std::endl;
            SYSTEM_STREAM << "\nSEND MOB: " << newmob.ret_id() << "\n";
            IMainItem::fabric->saveMap(str);
            IMainItem::mobMaster->map_access.unlock();
            NetMain::SendToStrSock(socket(), str);
            NetMaster->qmut.lock();
            //Add messages, what in map, but isn't in new client
            //
        }
        NetMaster->queues.push_back(&sendqueue_);
        NetMaster->qmut.unlock();
        NetMaster->PermitSend();
        NetMaster->SendRand();
        //std::string ls = loc1.str() + " rand";
        if(str.fail())
        {
            SYSTEM_STREAM << "Something went wrong, when we send our map\n";
            return;
        }
        RecvConnection cnct;
        cnct.connect = this;
        cnct.ptr = newmob.ret_id();
        SYSTEM_STREAM << newmob.ret_id() << " this is new id\n";
        cnct.strsock = &socket();
        Poco::Thread th;
        th.start(cnct);
        lock.unlock();
        std::cout << "\nSERVER: begin send msg\n";
        while(th.isRunning())
        {
            std::stringstream s;
            s << sendqueue_.Get(); 
            NetMain::SendToStrSock(socket(), s);
        }
        std::cout << "Lost connection\n";
        NetMaster->qmut.lock();
        NetMaster->queues.erase(std::find(NetMaster->queues.begin(), NetMaster->queues.end(), &sendqueue_));
        NetMaster->qmut.unlock();
    }
    catch(Poco::Exception& e)
    {
        SYSTEM_STREAM << e.displayText() << "\nvoid KvServerConnection::run()\n";
        std::cout << "Lost connection\n";
        NetMaster->qmut.lock();
        NetMaster->queues.erase(std::find(NetMaster->queues.begin(), NetMaster->queues.end(), &sendqueue_));
        NetMaster->qmut.unlock();
        return;
    }
}