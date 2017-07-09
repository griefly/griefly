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

#include "NetworkMessagesTypes.h"

const int MAX_WAIT_ON_QUEUE = 90;

struct Message
{
    qint32 type;
    QByteArray json;
};

Q_DECLARE_METATYPE(Message)

class Network2;

class SocketHandler : public QObject
{
    Q_OBJECT
public:
    SocketHandler(Network2* network);
public slots:
    void process();

    void tryConnect(QString host, int port, QString login, QString password);
    void socketConnected();

    void sendMessage(Message message);
    void disconnectSocket();
    void errorSocket(QAbstractSocket::SocketError error);
    void handleFirstMessage(Message message);
    void handleNewData();
signals:
    void firstMessage(Message message);
    void connectionEnd(QString reason);
    void readyToStart(int your_id, QString map);
private:
    void HandleSuccessConnection(Message m);

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

    static QJsonObject ParseJson(Message message);

    static bool IsKey(const QJsonObject& json, const QString& key);
    static quint32 ExtractObjId(const QJsonObject& json);
    static QString ExtractAction(const QJsonObject& json);

    static Message MakeClickMessage(int object_id, QString click_type);

    static Network2& GetInstance();

    bool IsGood();

    void TryConnect(QString host, int port, QString login, QString password);

    void SendMsg(Message message);
    void SendOrdinaryMessage(QString text);
    void SendPing(QString ping_id);

    void Disconnect();

    bool IsMessageAvailable();
    void WaitForMessageAvailable();
    Message PopMessage();

    QByteArray GetMapData();
public slots:
    void sendMap(QString url, QByteArray data);
    void onConnectionEnd(QString reason);
signals:
    void mapSendingStarted();
    void mapSendingFinished();
    void connectRequested(QString host, int port, QString login, QString password);
    void sendMessage(Message message);
    void disconnectRequested();
    void connectionSuccess(int your_id, QString map);
    void connectionFailed(QString reason);
private slots:
    void mapDownloaded(QNetworkReply* reply);
    void downloadMap(int your_id, QString map);
private:
    bool is_good_;

    bool prefer_compress_;

    int your_id_;
    QString map_url_;

    void PushMessage(Message message);

    QMutex queue_mutex_;
    QWaitCondition queue_wait_;

    QQueue<Message> received_messages_;

    Network2();

    SocketHandler handler_;
    QThread thread_;

    QNetworkAccessManager* net_manager_;
    QByteArray map_data_;
};
