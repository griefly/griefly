#include "Network2.h"

Network2 &Network2::GetInstance()
{
    static Network2* network = new Network2;
    return *network;
}

void Network2::PushMessage(Message message)
{

}

Network2::Network2()
    : reader_(this)
{

}


SocketReader::SocketReader(Network2* network)
    : network_(network)
{

}

void SocketReader::process()
{

}
