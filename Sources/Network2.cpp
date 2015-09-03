#include "Network2.h"

#include <QMutexLocker>
#include <QtEndian>

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
    qint32 message_size;
    qint32 message_type;
    QByteArray body;

    QByteArray old_data;

    enum ReadingState
    {
        SIZE, TYPE, BODY
    } state = ReadingState::SIZE;

    while (network_->socket_.isValid())
    {
        if (!network_->socket_.waitForReadyRead(1000))
        {
            continue;
        }

        QByteArray new_data = network_->socket_.readAll();

        if (new_data.size() == 0)
        {
            continue;
        }

        old_data.append(new_data);

        switch (state)
        {
        case ReadingState::SIZE:
            if (old_data.size() >= 4)
            {
                QByteArray loc = old_data.mid(0, 4);
                message_size = qFromBigEndian<qint32>((const uchar*)loc.data());
                if (old_data.size() == 4)
                {
                    old_data.clear();
                }
                else
                {
                    old_data = old_data.mid(4);
                }
                state = ReadingState::TYPE;
            }
            break;
        }
    }

}
