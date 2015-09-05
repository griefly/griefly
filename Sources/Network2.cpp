#include "Network2.h"

#include <QMutexLocker>
#include <QtEndian>

#include <QJsonObject>
#include <QJsonDocument>

#include "NetworkMessagesTypes.h"

Network2 &Network2::GetInstance()
{
    static Network2* network = new Network2;
    return *network;
}

Network2::Network2()
    : reader_(this)
{
    net_codec_ = QTextCodec::codecForName("UTF-8");

    connect(&socket_, &QTcpSocket::connected, this, &Network2::socketConnected);
}

void Network2::Connect(QString host, int port, QString login, QString password)
{
    socket_.connectToHost(host, port);
}

#define KV_XSTR(x) #x
#define KV_STR(x) KV_XSTR(x)

void Network2::socketConnected()
{
    SendData("S132");

    Message2 login_message;
    login_message.type = MessageType::INITAL_LOGIN_MESSAGE;

    QJsonObject obj;
    obj["login"] = QString("Guest");
    obj["password"] = QString("");

    // It is compile time macro with version (/D or -D)
    obj["game_version"] = QString(KV_STR(DEFINED_VERSION));

    QJsonDocument doc(obj);

    login_message.json = doc.toJson(QJsonDocument::Compact);


}

void Network2::socketError()
{

}

void Network2::SendData(const QByteArray &data)
{
    int counter = 0;
    while (counter != data.length() && socket_.isValid())
    {
        counter += socket_.write(data.data() + counter, data.length() - counter);
    }
}

void Network2::SendMessage(Message2 message)
{
    QByteArray json = net_codec_->fromUnicode(message.json);

    QByteArray data;

    uchar temp[4];

    qToBigEndian(json.size(), temp);
    data.append((char*)temp, 4);

    qToBigEndian(message.type, temp);
    data.append((char*)temp, 4);

    data.append(json);

    SendData(data);
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

SocketReader::SocketReader(Network2* network)
    : network_(network),
      state_(ReadingState::HEADER)
{

}

void SocketReader::process()
{
    while (network_->socket_.isValid())
    {
        if (!network_->socket_.waitForReadyRead(200))
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
    new_message.json = network_->net_codec_->toUnicode(loc);

    network_->PushMessage(new_message);

    if (buffer_.size() == HEADER_SIZE + message_size_)
    {
        buffer_.clear();
    }
    else
    {
        buffer_ = buffer_.mid(HEADER_SIZE + message_size_);
    }

    state_ = ReadingState::HEADER;
}
