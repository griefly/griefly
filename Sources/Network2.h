#pragma once

#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QThread>
#include <QQueue>
#include <QMutex>

struct Message
{
    qint32 type;
    QString json;
};

class Network2;

class SocketReader: public QObject
{
    Q_OBJECT
public:
    SocketReader(Network2* network);
public slots:
    void process();
private:
    Network2* network_;
};

class Network2: public QObject
{
    Q_OBJECT
public:
    static Network2& GetInstance();

    bool IsGood();

    void Connect(QString host, int port, QString login, QString password);

    void SendMessage(const Message& message);

    Message PopMessage();

    void Disconnect();

    void PushMessage(Message message);

private:  
    QMutex queue_mutex_;
    QQueue<Message> received_messages_;

    Network2();

    QTcpSocket socket_;

    enum NetworkState
    {
        NOT_CONNECTED, CONNECTING, CONNECTED, DISCONNECTED
    };
    NetworkState state_;

    SocketReader reader_;
};
