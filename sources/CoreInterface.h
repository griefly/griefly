#pragma once

#include <QByteArray>
#include <QVector>

namespace kv
{

class CoreInterface
{
public:
    class Frame
    {
        // TODO:
    };

    class Message
    {
        // TODO:
    };

    virtual void LoadWorld(const QByteArray& data) = 0;
    virtual void SaveWorld(QByteArray* data) const = 0;

    virtual void LoadWorldFromMapgen(const QByteArray& data) = 0;

    virtual quint32 Hash() const = 0;

    // TODO: Look into #360 properly
    virtual void ProcessNextTick(const QVector<Message>& messages);

    virtual void Represent(Frame* frame) const;

    // TODO: metadata for map editor?
};

}
