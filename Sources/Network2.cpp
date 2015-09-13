#include "Network2.h"

#include <QMutexLocker>
#include <QtEndian>

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonValueRef>
#include <QVariant>

#include <QCoreApplication>

#include "NetworkMessagesTypes.h"

Network2 &Network2::GetInstance()
{
    static Network2* network = new Network2;
    return *network;
}

Network2::Network2()
    : handler_(this)
{
    handler_.moveToThread(&thread_);
    connect(&thread_, &QThread::started, &handler_, &SocketHandler::process);

    connect(this, &Network2::connectRequested, &handler_, &SocketHandler::tryConnect);
}

void Network2::TryConnect(QString host, int port, QString login, QString password)
{
    thread_.start();
    emit connectRequested(host, port, login, password);
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

SocketHandler::SocketHandler(Network2* network)
    : network_(network),
      socket_(this)
{
    state_ = NetworkState::NOT_CONNECTED;

    net_codec_ = QTextCodec::codecForName("UTF-8");

    connect(&socket_, &QTcpSocket::connected, this, &SocketHandler::socketConnected);
    connect(&socket_,
            static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error),
            this,
            &SocketHandler::errorSocket);

    connect(this, &SocketHandler::firstMessage, this, &SocketHandler::handleFirstMessage);
    connect(&socket_, &QTcpSocket::readyRead, this, &SocketHandler::handleNewData);
}

void SocketHandler::process()
{
    reading_state_ = ReadingState::HEADER;
    is_first_message_ = true;

    while (state_ != NetworkState::DISCONNECTED)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 40);
    }

    qDebug() << "Socket no longer valid";
}

void SocketHandler::handleNewData()
{
    qDebug() << "handleNewData()";

    QByteArray new_data = socket_.readAll();

    buffer_.append(new_data);

    switch (reading_state_)
    {
    case ReadingState::HEADER:
        HandleHeader();
        break;
    case ReadingState::BODY:
        HandleBody();
        break;
    }
}

const int HEADER_SIZE = 8;

void SocketHandler::HandleHeader()
{
    if (buffer_.size() < HEADER_SIZE)
    {
        return;
    }
    QByteArray loc = buffer_.mid(0, 4);
    message_size_ = qFromBigEndian<qint32>((const uchar*)loc.data());

    loc = buffer_.mid(4, 4);
    message_type_ = qFromBigEndian<qint32>((const uchar*)loc.data());

    reading_state_ = ReadingState::BODY;
    HandleBody();
}

void SocketHandler::HandleBody()
{
    if (buffer_.size() < HEADER_SIZE + message_size_)
    {
        return;
    }

    Message2 new_message;
    new_message.type = message_type_;

    QByteArray loc = buffer_.mid(HEADER_SIZE, message_size_);
    new_message.json = net_codec_->toUnicode(loc);

    qDebug() << new_message.json;

    network_->PushMessage(new_message);

    if (buffer_.size() == HEADER_SIZE + message_size_)
    {
        buffer_.clear();
    }
    else
    {
        buffer_ = buffer_.mid(HEADER_SIZE + message_size_);
    }

    reading_state_ = ReadingState::HEADER;

    if (is_first_message_)
    {
        qDebug() << "First message";
        emit firstMessage();
        is_first_message_ = false;
    }

    HandleHeader();
}


void SocketHandler::tryConnect(QString host, int port, QString login, QString password)
{
    login_ = login;
    password_ = password;
    socket_.connectToHost(host, port);

    qDebug() << "tryConnect()";
}

#define KV_XSTR(x) #x
#define KV_STR(x) KV_XSTR(x)

void SocketHandler::socketConnected()
{
    qDebug() << "socketConnected()";

    SendData("S131");

    Message2 login_message;
    login_message.type = MessageType::INITAL_LOGIN_MESSAGE;

    QJsonObject obj;
    obj["login"] = login_;
    obj["password"] = password_;

    // It is compile time macro with version (/D or -D)
    obj["game_version"] = QString("v0.1.1-100-gf12e");//QString(KV_STR(DEFINED_VERSION));

    QJsonDocument doc(obj);

    login_message.json = doc.toJson();

    sendMessage(login_message);

    state_ = NetworkState::CONNECTED;
}

void SocketHandler::sendMessage(Message2 message)
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

void SocketHandler::disconnectSocket()
{

}

void SocketHandler::errorSocket(QAbstractSocket::SocketError error)
{
    qDebug() << "socketError()";
    qDebug() << error;
    //thread_.wait();
    state_ = NetworkState::NOT_CONNECTED;
    emit connectionEnd();
}

void SocketHandler::handleFirstMessage()
{
    Message2 m = network_->PopMessage();

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

void SocketHandler::HandleSuccessConnection(Message2 message)
{
    QJsonDocument doc = QJsonDocument::fromJson(message.json.toUtf8());
    QJsonObject obj = doc.object();

    qDebug() << obj["map"];
    qDebug() << obj["your_id"];

    map_ = obj["map"].toString();

    QJsonValue val = obj["your_id"];
    your_id_ = val.toVariant().toInt();

    if (map_ == "no_map")
    {
        emit readyToStart(your_id_);
    }
    else
    {
        emit downloadMapRequest();
    }
}

void SocketHandler::SendData(const QByteArray &data)
{
    int counter = 0;
    while (counter != data.length() && socket_.isValid())
    {
        counter += socket_.write(data.data() + counter, data.length() - counter);
    }
}
