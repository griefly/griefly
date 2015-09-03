#include "Network2.h"

#include <QMutexLocker>

Network2 &Network2::GetInstance()
{
    static Network2* network = new Network2;
    return *network;
}

void Network2::PushMessage(Message message)
{
    QMutexLocker locker(&queue_mutex_);

    received_messages_.enqueue(message);
}

Message Network2::PopMessage()
{
    QMutexLocker locker(&queue_mutex_);

    return received_messages_.dequeue();
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
    while (network_->socket_.isValid())
    {
        network_->socket_.waitForReadyRead();

        QByteArray data = network_->socket_.readAll();

        // TODO
    }

}
