#pragma once

#include <memory>
#include <limits>

#include <QByteArray>
#include <QJsonObject>
#include <QMap>
#include <QString>
#include <QVector>

#include "RawViewInfo.h"
#include "Dir.h"
#include "Messages.h"

namespace kv
{

struct FrameData
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

        kv::RawViewInfo view;
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

        kv::RawViewInfo view;
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

    FrameData()
        : camera_pos_x(0),
          camera_pos_y(0)
    {
        // Nothing
    }

    QVector<Entity> entities;
    QVector<Sound> sounds;
    QVector<InterfaceUnit> units;
    QVector<ChatMessage> messages;
    QVector<TextEntry> texts;
    Music music;
    int camera_pos_x;
    int camera_pos_y;
};

class GrowingFrame
{
public:
    GrowingFrame(FrameData* data)
        : frame_data_(data)
    {
        // Nothing
    }
    void Append(const FrameData::Entity& entity)
    {
        frame_data_->entities.append(entity);
    }
    void Append(const FrameData::Sound& sound)
    {
        frame_data_->sounds.append(sound);
    }
    void Append(const FrameData::InterfaceUnit& unit)
    {
        frame_data_->units.append(unit);
    }
    void Append(const FrameData::ChatMessage& message)
    {
        frame_data_->messages.append(message);
    }
    void Append(const FrameData::TextEntry& text)
    {
        frame_data_->texts.append(text);
    }
    void SetMusic(const FrameData::Music& music)
    {
        frame_data_->music = music;
    }
    void SetCamera(int posx, int posy)
    {
        frame_data_->camera_pos_x = posx;
        frame_data_->camera_pos_y = posy;
    }
    quint32 GetUniqueIdForNewFrame(quint32 base_id, quint32 number)
    {
        // TODO: There should be better way to do it
        const quint32 MAX_NUMBER = 32;
        const quint32 MAX_BASE_ID = std::numeric_limits<quint32>::max() / MAX_NUMBER;
        return base_id + MAX_BASE_ID * number;
    }

private:
    FrameData* frame_data_;
};

class WorldInterface
{
public:
    virtual ~WorldInterface() { }

    // TODO: FastSerializer should be able to serialize right into QByteArray
    virtual QByteArray SaveWorld() const = 0;

    // TODO: Look into #360 properly
    virtual void StartTick() = 0;
    virtual void ProcessMessage(const Message& messages) = 0;
    virtual void FinishTick() = 0;

    using PlayerAndFrame = std::pair<quint32, GrowingFrame*>;
    virtual void Represent(const QVector<PlayerAndFrame>& frames) const = 0;
    virtual qint32 GetGameTick() const = 0;
    virtual quint32 Hash() const = 0;

    virtual void PerformUnsync() = 0;

};

class CoreInterface
{
public:
    struct ObjectMetadata
    {
        QString name;
        RawViewInfo default_view;
    };
    struct Config
    {
        bool unsync_generation;
    };

    virtual ~CoreInterface() { }

    using WorldPtr = std::shared_ptr<WorldInterface>;

    virtual WorldPtr CreateWorldFromSave(
        const QByteArray& data, quint32 mob_id) = 0;
    virtual WorldPtr CreateWorldFromJson(
        const QJsonObject& data, quint32 mob_id, const Config& config) = 0;
    // <object name, metadata>
    using ObjectsMetadata = QMap<QString, ObjectMetadata>;
    virtual const ObjectsMetadata& GetObjectsMetadata() const = 0;
};

CoreInterface& GetCoreInstance();

}
