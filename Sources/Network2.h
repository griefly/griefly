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

class SocketReader: public QObject
{
    Q_OBJECT
public:
    SocketReader(Network2* network);
signals:
    void firstMessage();
public slots:
    void process();
private:
    bool is_first_message_;

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

    void TryConnect(QString host, int port, QString login, QString password);

    void SendMessage(Message2 message);

    void Disconnect();

    bool IsMessageAvailable();
    Message2 PopMessage();
public slots:
    void socketConnected();
    void socketError();

    void firstMessage();

signals:
    void connected();

private:
    QString login_;
    QString password_;

    void SendData(const QByteArray& data);

    QTextCodec* net_codec_;

    void PushMessage(Message2 message);

    QMutex queue_mutex_;
    QQueue<Message2> received_messages_;

    Network2();

    QTcpSocket socket_;

    enum NetworkState
    {
        NOT_CONNECTED, CONNECTING, CONNECTED, DISCONNECTED
    };
    NetworkState state_;

    SocketReader reader_;
    QThread thread_;
};
