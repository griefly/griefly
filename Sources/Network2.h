#pragma once

#include <QObject>
#include <QString>
#include <QTcpSocket>

#include <QQueue>

struct Message
{
    qint32 type;
    QString json;
};

class Network2: public QObject
{
    Q_OBJECT
public:
    static Network2& GetInstance();

    bool IsGood();

    void Connect(QString host, int port, QString login, QString password);

    void SendMessage(const Message& message);

    Message RecvMessage();

    void Disconnect();

public slots:
    void pushReceivedMessage(Message message);

private:
    QQueue<Message> received_messages_;

    Network2();

    QTcpSocket socket_;

    enum NetworkState
    {
        NOT_CONNECTED, CONNECTING, CONNECTED, DISCONNECTED
    };
    NetworkState state_;
};
