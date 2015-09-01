#include "Network2.h"


Network2 &Network2::GetInstance()
{
    static Network2* network = new Network2;
    return *network;
}

void Network2::pushReceivedMessage(Message message)
{

}

Network2::Network2()
{

}
