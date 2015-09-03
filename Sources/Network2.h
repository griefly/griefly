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
    qint32 message_size_;
    qint32 message_type_;

    QByteArray buffer_;

    enum ReadingState
    {
        HEADER, BODY
    } state_;

    void HandleHeader();
    void HandleBody();

    Network2* network_;
};

class Network2: public QObject
{
    Q_OBJECT
public:
    friend class SocketReader;

    static Network2& GetInstance();

    bool IsGood();

    void Connect(QString host, int port, QString login, QString password);

    void SendMessage(const Message& message);

    void Disconnect();

    void PushMessage(Message message);
    Message PopMessage();

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
