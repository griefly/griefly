#pragma once

#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QThread>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include <QTextCodec>
#include <QJsonObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <Messages.h>

const int MAX_WAIT_ON_QUEUE = 90;

Q_DECLARE_METATYPE(kv::Message)

class Network2;

class SocketHandler : public QObject
{
    Q_OBJECT
public:
    SocketHandler(Network2* network);
public slots:
    void process();

    void tryConnect(
        const QString& host, int port, const QString& login, const QString& password);
    void socketConnected();

    void sendMessage(const kv::Message& message);
    void disconnectSocket();
    void errorSocket(QAbstractSocket::SocketError error);
    void handleFirstMessage(const kv::Message& message);
    void handleNewData();
signals:
    void firstMessage(const kv::Message& message);
    void connectionEnd(const QString& reason);
    void readyToStart(int your_id, QString map);
private:
    void HandleSuccessConnection(const kv::Message& message);

    QTcpSocket socket_;

    void SendData(const QByteArray& data);

    QString login_;
    QString password_;

    QString map_;
    int your_id_;

    bool is_first_message_;

    qint32 message_size_;
    qint32 message_type_;

    QByteArray buffer_;
    int buffer_pos_;

    enum ReadingState
    {
        HEADER, BODY
    } reading_state_;

    enum NetworkState
    {
        NOT_CONNECTED, CONNECTING, CONNECTED, DISCONNECTED
    };
    NetworkState state_;

    bool HandleHeader();
    bool HandleBody();

    QString possible_error_reason_;

    Network2* network_;
};

class Network2 : public QObject
{
    Q_OBJECT
public:
    friend class SocketHandler;

    static bool IsKey(const QJsonObject& json, const QString& key)
    {
        if (json["key"] == key)
        {
            return true;
        }
        return false;
    }
    static quint32 ExtractObjId(const QJsonObject& json)
    {
        return json["obj"].toInt();
    }
    static QString ExtractAction(const QJsonObject& json)
    {
        return json["action"].toString();
    }

    static kv::Message MakeClickMessage(int object_id, const QString& click_type)
    {
        kv::Message message;

        message.type = kv::message_type::MOUSE_CLICK;
        message.data = {{"action", click_type}, {"obj", object_id}};

        return message;
    }

    static Network2& GetInstance();

    bool IsGood() const;

    void TryConnect(
        const QString& host, int port, const QString& login, const QString& password);

    void Send(const kv::Message& message);
    void SendOrdinaryMessage(const QString& text);
    void SendPing(const QString& ping_id);

    void Disconnect();

    bool IsMessageAvailable() const;
    void WaitForMessageAvailable() const;
    kv::Message PopMessage();

    QByteArray GetMapData() const;
public slots:
    void sendMap(const QString& url, const QByteArray& data);
    void onConnectionEnd(const QString& reason);
signals:
    void mapSendingStarted();
    void mapSendingFinished();
    void connectRequested(
        const QString& host, int port, const QString& login, const QString& password);
    void sendMessage(const kv::Message& message);
    void disconnectRequested();
    void connectionSuccess(int your_id, const QString& map);
    void connectionFailed(const QString& reason);
private slots:
    void mapDownloaded(QNetworkReply* reply);
    void downloadMap(int your_id, const QString& map);
private:
    bool is_good_;

    bool prefer_compress_;

    int your_id_;
    QString map_url_;

    void PushMessage(const kv::Message& message);

    mutable QMutex queue_mutex_;
    mutable QWaitCondition queue_wait_;

    QQueue<kv::Message> received_messages_;

    Network2();

    SocketHandler handler_;
    QThread thread_;

    QNetworkAccessManager* net_manager_;
    QByteArray map_data_;
};
