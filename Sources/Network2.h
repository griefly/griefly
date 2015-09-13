#pragma once

#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QThread>
#include <QQueue>
#include <QMutex>
#include <QTextCodec>
#include <QByteArray>

struct Message2
{
    qint32 type;
    QString json;
};

class Network2;

class SocketHandler: public QObject
{
    Q_OBJECT
public:
    SocketHandler(Network2* network);
public slots:
    void process();

    void tryConnect(QString host, int port, QString login, QString password);
    void socketConnected();

    void sendMessage(Message2 message);
    void disconnectSocket();
    void errorSocket(QAbstractSocket::SocketError error);
    void handleFirstMessage();
    void handleNewData();
signals:
    void firstMessage();
    void connectionEnd();
    void readyToStart(int your_id);
    void downloadMapRequest();
private:
    void HandleSuccessConnection(Message2 m);

    QTcpSocket socket_;

    void SendData(const QByteArray& data);

    QTextCodec* net_codec_;

    QString login_;
    QString password_;

    QString map_;
    int your_id_;

    bool is_first_message_;

    qint32 message_size_;
    qint32 message_type_;

    QByteArray buffer_;

    enum ReadingState
    {
        HEADER, BODY
    } reading_state_;

    enum NetworkState
    {
        NOT_CONNECTED, CONNECTING, CONNECTED, DISCONNECTED
    };
    NetworkState state_;

    void HandleHeader();
    void HandleBody();

    Network2* network_;
};

class Network2: public QObject
{
    Q_OBJECT
public:
    friend class SocketHandler;

    static Network2& GetInstance();

    bool IsGood();

    void TryConnect(QString host, int port, QString login, QString password);

    void SendMessage(Message2 message);

    void Disconnect();

    bool IsMessageAvailable();
    Message2 PopMessage();
signals:
    void connectRequested(QString host, int port, QString login, QString password);
    void sendMessage(Message2 message);
    void disconnectRequested();

private:
    void PushMessage(Message2 message);

    QMutex queue_mutex_;
    QQueue<Message2> received_messages_;

    Network2();

    SocketHandler handler_;
    QThread thread_;
};
