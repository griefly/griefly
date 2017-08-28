#pragma once

#include <memory>

#include <QByteArray>
#include <QJsonObject>
#include <QMap>
#include <QString>
#include <QVector>

namespace kv
{

class WorldInterface
{
public:
    class Frame
    {
        // TODO:
    };

    class Message
    {
        qint32 id;
        QJsonObject data;
    };

    virtual ~WorldInterface() { }

    virtual void SaveWorld(QByteArray* data) const = 0;

    // TODO: Look into #360 properly
    virtual void ProcessNextTick(const QVector<Message>& messages) = 0;

    virtual void Represent(Frame* frame) const = 0;
    virtual quint32 Hash() const = 0;

};

class CoreInterface
{
public:
    struct ObjectMetadata
    {
        QString sprite;
        QString state;
    };

    virtual ~CoreInterface() { }

    using WorldPtr = std::shared_ptr<WorldInterface>;

    virtual WorldPtr CreateWorldFromSave(const QByteArray& data) = 0;
    virtual WorldPtr CreateWorldFromMapgen(const QByteArray& data) = 0;

    // <object name, metadata>
    virtual const QMap<QString, ObjectMetadata>& GetObjectsMetadata() const = 0;
};

CoreInterface& GetCoreInstance();

}
