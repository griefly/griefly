#pragma once

#include <QObject>
#include <QString>

struct Message
{
    qint32 type;
    QString json;
};

class Network2: public QObject
{
    Q_OBJECT
public:
    // such code much technology
};
