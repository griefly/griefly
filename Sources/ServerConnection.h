#pragma once

#include <Poco/Net/TCPServerConnection.h>

#include "SyncQueue.h"

class RecvConnection;

class KvServerConnection: public Poco::Net::TCPServerConnection
{
public:
	KvServerConnection(const Poco::Net::StreamSocket& s): TCPServerConnection(s)
	{
	}
    friend RecvConnection;
    virtual void run();
private:
    SynchronizedQueue<std::string> sendqueue_;
};