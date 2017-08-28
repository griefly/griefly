#pragma once

#include <memory>

#include <QByteArray>
#include <QJsonObject>
#include <QMap>
#include <QString>
#include <QVector>

#include "representation/ViewInfo.h"
#include "core/Constheader.h"

namespace kv
{

class Frame
{
    struct Entity
    {
        Entity()
            : id(0),
              click_id(0),
              pos_x(0),
              pos_y(0),
              vlevel(0),
              dir(Dir::SOUTH)
        {
            // Nothing
        }

        ViewInfo view;
        quint32 id;
        quint32 click_id;
        int pos_x;
        int pos_y;
        int vlevel;
        Dir dir;
    };

    struct InterfaceUnit
    {
        InterfaceUnit()
            : pixel_x(0),
              pixel_y(0),
              shift(0)
        {
            // Nothing
        }

        ViewInfo view;
        QString name;

        int pixel_x;
        int pixel_y;

        int shift;
    };

    struct Sound
    {
        QString name;
    };

    struct ChatMessage
    {
        QString html;
    };

    struct TextEntry
    {
        QString tab;
        QString text;
    };

    struct Music
    {
        Music()
            : volume(0)
        {
            // Nothing
        }

        QString name;
        int volume;
    };
};

class WorldInterface
{
public:
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
