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
    state_ = NetworkState::NOT_CONNECTED;

    net_codec_ = QTextCodec::codecForName("UTF-8");

    connect(&socket_, &QTcpSocket::connected, this, &Network2::socketConnected);
    connect(&socket_,
            static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error),
            this,
            &Network2::socketError);

    reader_.moveToThread(&thread_);
    connect(&thread_, &QThread::started, &reader_, &SocketReader::process);

    connect(&reader_, &SocketReader::firstMessage, this, &Network2::firstMessage);
}

void Network2::TryConnect(QString host, int port, QString login, QString password)
{
    login_ = login;
    password_ = password;
    socket_.connectToHost(host, port);

    qDebug() << "TryConnect()";
}

#define KV_XSTR(x) #x
#define KV_STR(x) KV_XSTR(x)

void Network2::socketConnected()
{
    qDebug() << "socketConnected()";
    state_ = NetworkState::CONNECTING;

    SendData("S132");

    thread_.start();

    Message2 login_message;
    login_message.type = MessageType::INITAL_LOGIN_MESSAGE;

    QJsonObject obj;
    obj["login"] = login_;
    obj["password"] = password_;

    // It is compile time macro with version (/D or -D)
    obj["game_version"] = QString(KV_STR(DEFINED_VERSION));

    QJsonDocument doc(obj);

    login_message.json = doc.toJson();

    SendMessage(login_message);
}

void Network2::socketError()
{
    qDebug() << "socketError()";
    thread_.wait();
    state_ = NetworkState::NOT_CONNECTED;
}

void Network2::firstMessage()
{
    Message2 m = PopMessage();

    switch (m.type)
    {
    case MessageType::WRONG_GAME_VERSION:
        qDebug() << "Wrong game version";
        qDebug() << m.json;
        break;
    case MessageType::WRONG_AUTHENTICATION:
        qDebug() << "Wrong authentication";
        qDebug() << m.json;
        break;
    case MessageType::SUCCESS_CONNECTION:
        qDebug() << "Success";
        qDebug() << m.json;
        HandleSuccessConnection(m);
        break;
    default:
        qDebug() << "Unknown message type: " << m.type;
        qDebug() << m.json;
        break;
    }
}

void Network2::HandleSuccessConnection(Message2 message)
{
    QJsonDocument doc = QJsonDocument::fromJson(message.json.toUtf8());
    QJsonObject obj = doc.object();

    qDebug() << obj["map"];
    qDebug() << obj["your_id"];
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

bool Network2::IsMessageAvailable()
{
    QMutexLocker locker(&queue_mutex_);

    return received_messages_.size() > 0;
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
    : network_(network)
{

}

void SocketReader::process()
{
    state_ = ReadingState::HEADER;
    is_first_message_ = true;

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

    if (is_first_message_)
    {
        emit firstMessage();
        is_first_message_ = false;
    }
}
