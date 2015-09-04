#include "Network2.h"

#include <QMutexLocker>
#include <QtEndian>

Network2 &Network2::GetInstance()
{
    static Network2* network = new Network2;
    return *network;
}

void Network2::PushMessage(Message2 message)
{
    QMutexLocker locker(&queue_mutex_);

    received_messages_.enqueue(message);
}

Message2 Network2::PopMessage()
{
    QMutexLocker locker(&queue_mutex_);

    return received_messages_.dequeue();
}

Network2::Network2()
    : reader_(this)
{

}


SocketReader::SocketReader(Network2* network)
    : network_(network),
      state_(ReadingState::HEADER)
{
    utf_codec_ = QTextCodec::codecForName("UTF-8");
}

void SocketReader::process()
{
    while (network_->socket_.isValid())
    {
        if (!network_->socket_.waitForReadyRead(1000))
        {
            continue;
        }

        QByteArray new_data = network_->socket_.readAll();

        buffer_.append(new_data);

        switch (state_)
        {
        case ReadingState::HEADER:
            HandleHeader();
            break;
        case ReadingState::BODY:
            HandleBody();
            break;
        }
    }

}

const int HEADER_SIZE = 8;

void SocketReader::HandleHeader()
{
    if (buffer_.size() > HEADER_SIZE)
    {
        return;
    }
    QByteArray loc = buffer_.mid(0, 4);
    message_size_ = qFromBigEndian<qint32>((const uchar*)loc.data());

    loc = buffer_.mid(4, 4);
    message_type_ = qFromBigEndian<qint32>((const uchar*)loc.data());

    state_ = ReadingState::BODY;
}

void SocketReader::HandleBody()
{
    if (buffer_.size() < HEADER_SIZE + message_size_)
    {
        return;
    }

    Message2 new_message;
    new_message.type = message_type_;

    QByteArray loc = buffer_.mid(HEADER_SIZE, message_size_);
    new_message.json = utf_codec_->toUnicode(loc);

    network_->PushMessage(new_message);

    if (buffer_.size() == HEADER_SIZE + message_size_)
    {
        buffer_.clear();
    }
    else
    {
        buffer_ = buffer_.mid(HEADER_SIZE + message_size_);
    }
}
