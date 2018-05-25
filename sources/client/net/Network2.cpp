#include "Network2.h"

#include <QMutexLocker>
#include <QtEndian>

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonValueRef>
#include <QVariant>
#include <QUrl>
#include <QElapsedTimer>

#include <QCoreApplication>

#include <CoreInterface.h>

using kv::Message;

Network2& Network2::GetInstance()
{
    static Network2* network = new Network2;
    return *network;
}

bool Network2::IsGood() const
{
    return is_good_;
}

void Network2::sendMap(const QString& url, const QByteArray& data_raw)
{
    QString type_header = "application/octet-stream";
    QByteArray data = data_raw;
    if (prefer_compress_)
    {
        type_header = "application/zip";
        data = qCompress(data);
    }

    QNetworkRequest request(QUrl{url});
    request.setHeader(QNetworkRequest::ContentLengthHeader, data.length());
    request.setHeader(QNetworkRequest::ContentTypeHeader, type_header);

    emit mapSendingStarted();
    net_manager_->post(request, data);

    qDebug() << "Map has been sended to " << url << ", length: " << data.length();
}

Network2::Network2()
    : handler_(this)
{
    prefer_compress_ = true;

    net_manager_ = new QNetworkAccessManager(this);

    handler_.moveToThread(&thread_);
    connect(&thread_, &QThread::started, &handler_, &SocketHandler::process);

    connect(net_manager_, &QNetworkAccessManager::finished, this, &Network2::mapDownloaded);

    connect(this, &Network2::connectRequested, &handler_, &SocketHandler::tryConnect);
    connect(&handler_, &SocketHandler::readyToStart, this, &Network2::downloadMap);
    connect(this, &Network2::sendMessage, &handler_, &SocketHandler::sendMessage);
    connect(&handler_, &SocketHandler::connectionEnd, this, &Network2::onConnectionEnd);
    connect(this, &Network2::disconnectRequested, &handler_, &SocketHandler::disconnectSocket);
}

void Network2::TryConnect(
    const QString& host, int port, const QString& login, const QString& password)
{
    thread_.start();
    emit connectRequested(host, port, login, password);
}

void Network2::Send(const kv::Message& message)
{
    emit sendMessage(message);
}

void Network2::SendOrdinaryMessage(const QString& text)
{
    Message message;
    message.type = kv::message_type::ORDINARY;
    message.data = {{"key", text}};

    Send(message);
}

void Network2::SendPing(const QString& ping_id)
{
    Message message;
    message.type = kv::message_type::PING;
    message.data = {{"ping_id", ping_id}};

    Send(message);
}

void Network2::Disconnect()
{
    emit disconnectRequested();
    thread_.wait();
}

bool Network2::IsMessageAvailable() const
{
    QMutexLocker locker(&queue_mutex_);
    return received_messages_.size() > 0;
}

void Network2::WaitForMessageAvailable() const
{
    QMutexLocker locker(&queue_mutex_);

    if (received_messages_.size() == 0)
    {
        queue_wait_.wait(&queue_mutex_, MAX_WAIT_ON_QUEUE);
    }
}

Message Network2::PopMessage()
{
    QMutexLocker locker(&queue_mutex_);
    return received_messages_.dequeue();
}

void Network2::PushMessage(const kv::Message& message)
{
    QMutexLocker locker(&queue_mutex_);
    received_messages_.enqueue(message);
    queue_wait_.wakeOne();
}

QByteArray Network2::GetMapData() const
{
    return map_data_;
}

void Network2::onConnectionEnd(const QString& reason)
{
    is_good_ = false;
    emit connectionFailed(reason);
}

void Network2::mapDownloaded(QNetworkReply* reply)
{
    if (reply->request().url() != map_url_)
    {
        emit mapSendingFinished();
        qDebug() << "End map upload";
        reply->deleteLater();
        return;
    }

    if (reply->error() != QNetworkReply::NoError)
    {
        emit connectionFailed("Unable download map: " + reply->errorString());
        reply->deleteLater();
        return;
    }

    map_data_ = reply->readAll();
    if (reply->header(QNetworkRequest::ContentTypeHeader) == "application/zip")
    {
        qDebug() << "Compressed map length: " << map_data_.length();
        map_data_ = qUncompress(map_data_);
    }
    reply->deleteLater();

    qDebug() << "Map length: " << map_data_.length();

    is_good_ = true;
    emit connectionSuccess(your_id_, "map_buffer");
}

void Network2::downloadMap(int your_id, const QString& map)
{
    your_id_ = your_id;
    map_url_ = map;
    if (map == "no_map")
    {
        is_good_ = true;
        emit connectionSuccess(your_id, map);
        return;
    }

    qDebug() << "Begin download map from " << map_url_;

    const QNetworkRequest request(QUrl{map_url_});
    net_manager_->get(request);
}

SocketHandler::SocketHandler(Network2* network)
    : network_(network),
      socket_(this)
{    
    state_ = NetworkState::NOT_CONNECTED;

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
    buffer_pos_ = 0;
}

void SocketHandler::handleNewData()
{
    const QByteArray new_data = socket_.readAll();
    buffer_.append(new_data);

    bool is_continue = true;
    while (is_continue)
    {
        switch (reading_state_)
        {
        case ReadingState::HEADER:
            is_continue = HandleHeader();
            break;
        case ReadingState::BODY:
            is_continue = HandleBody();
            break;
        }
    }

    const int MAX_BUFFER_SIZE = 1024 * 128;
    if (buffer_.size() > MAX_BUFFER_SIZE)
    {
        buffer_.remove(0, buffer_pos_);
        buffer_pos_ = 0;
    }
}

const int HEADER_SIZE = 8;

bool SocketHandler::HandleHeader()
{
    if (buffer_.size() - buffer_pos_ < HEADER_SIZE)
    {
        return false;
    }
    message_size_ = qFromBigEndian<qint32>(
        reinterpret_cast<const uchar*>(&(buffer_.constData()[buffer_pos_])));
    message_type_ = qFromBigEndian<qint32>(
        reinterpret_cast<const uchar*>(&(buffer_.constData()[buffer_pos_ + 4])));

    buffer_pos_ += 8;
    reading_state_ = ReadingState::BODY;
    return true;
}

bool SocketHandler::HandleBody()
{
    if (buffer_.size() - buffer_pos_ < message_size_)
    {
        return false;
    }

    Message new_message;
    new_message.type = message_type_;

    QByteArray data;
    data.append(
        &buffer_.constData()[buffer_pos_],
        message_size_);
    buffer_pos_ += message_size_;

    // Dont check validation because server should always send correct json
    // TODO: check anyway?
    QJsonDocument document = QJsonDocument::fromJson(data);
    new_message.data = document.object();

    if (is_first_message_)
    {
        emit firstMessage(new_message);
        is_first_message_ = false;
    }
    else
    {
        network_->PushMessage(new_message);
    }
    reading_state_ = ReadingState::HEADER;

    return true;
}


void SocketHandler::tryConnect(
    const QString& host, int port, const QString& login, const QString& password)
{
    login_ = login;
    password_ = password;

    socket_.setSocketOption(QAbstractSocket::LowDelayOption, 1);

    socket_.connectToHost(host, port);

    qDebug() << "tryConnect()";
}

void SocketHandler::socketConnected()
{
    qDebug() << "socketConnected()";
    // TCP_NODELAY
    socket_.setSocketOption(QAbstractSocket::LowDelayOption, 1);

    SendData("S132");

    Message login_message;
    login_message.type = kv::message_type::INITAL_LOGIN_MESSAGE;

    QJsonObject object;
    object["login"] = login_;
    object["password"] = password_;

    object["game_version"] = QString(kv::GetCoreInstance().GetGameVersion());

    const bool is_guest = (login_ == "Guest");
    object["guest"] = is_guest;

    login_message.data = object;

    qDebug() << login_message.data;

    sendMessage(login_message);

    state_ = NetworkState::CONNECTED;
}

void SocketHandler::sendMessage(const kv::Message& message)
{
    const QJsonDocument document(message.data);
    const QByteArray json = document.toJson(QJsonDocument::Compact);

    QByteArray data;

    uchar temp[4];

    qToBigEndian(json.size(), temp);
    data.append(reinterpret_cast<char*>(temp), 4);

    qToBigEndian(message.type, temp);
    data.append(reinterpret_cast<char*>(temp), 4);

    data.append(json);

    SendData(data);
}

void SocketHandler::disconnectSocket()
{
    socket_.close();
    network_->thread_.exit(-1);
}

void SocketHandler::errorSocket(QAbstractSocket::SocketError error)
{
    qDebug() << "socketError()";
    qDebug() << error;
    state_ = NetworkState::NOT_CONNECTED;
    network_->thread_.exit(-1);
    emit connectionEnd(
        QString("Socket error: '%1' (%2), reason: %3")
            .arg(socket_.errorString())
            .arg(error)
            .arg(possible_error_reason_));
}

void SocketHandler::handleFirstMessage(const kv::Message& message)
{
    using namespace kv;

    switch (message.type)
    {
    case message_type::WRONG_GAME_VERSION:
        qDebug() << "Wrong game version";
        qDebug() << message.data;
        possible_error_reason_ = "Wrong game version: " + message.data["correct_game_version"].toString();
        break;
    case message_type::WRONG_AUTHENTICATION:
        qDebug() << "Wrong authentication";
        qDebug() << message.data;
        possible_error_reason_ = "Wrong authentication";
        break;
    case message_type::SUCCESS_CONNECTION:
        qDebug() << "Success";
        qDebug() << message.data;
        HandleSuccessConnection(message);
        break;
    case message_type::MASTER_CLIENT_IS_NOT_CONNECTED:
        qDebug() << "Master client is not connected";
        possible_error_reason_ = "Master client is not connected";
        break;
    case message_type::INTERNAL_SERVER_ERROR:
        qDebug() << "The server experiences some internal troubles";
        possible_error_reason_ = "The server experiences some internal troubles: " + message.data["message"].toString();
        break;
    default:
        qDebug() << "Unknown message type: " << message.type;
        qDebug() << message.data;
        possible_error_reason_ = "Unknown message type";
        break;
    }
}

void SocketHandler::HandleSuccessConnection(const kv::Message& message)
{
    qDebug() << message.data["map"];
    qDebug() << message.data["your_id"];

    map_ = message.data["map"].toString();
    your_id_ = message.data["your_id"].toInt();

    emit readyToStart(your_id_, map_);
}

void SocketHandler::SendData(const QByteArray &data)
{
    int counter = 0;
    while (counter != data.length() && socket_.isValid())
    {
        counter += socket_.write(data.data() + counter, data.length() - counter);
    }
}
