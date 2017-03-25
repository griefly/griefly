#include "Metadata.h"

#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonParseError>

#include "KVAbort.h"
#include "JsonValidator.h"

const ImageMetadata::SpriteMetadata& 
    ImageMetadata::GetSpriteMetadata(const QString& name)
{
    return sprites_metadata_[name];
}

bool ImageMetadata::IsValidState(const QString& name)
{
    if (!Valid())
    {
        return false;
    }
    return sprites_metadata_.find(name) != sprites_metadata_.end();
}

void ImageMetadata::Init(const QString& name, int width, int height)
{   
    qDebug() << "Begin to init metadata for " << name;

    width_ = width;
    height_ = height;

    QFile file(name + ".json");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Unable to open metadata file, trying without metadata: " << name;
        InitWithoutMetadata();
    }
    else
    {
        QByteArray data = file.readAll();
        QJsonParseError error;
        QJsonDocument document = QJsonDocument::fromJson(data, &error);
        if (error.error != QJsonParseError::NoError)
        {
            qDebug() << "Json parase error:" << error.errorString();
            KvAbort();
        }
        ParseInfo(document.object());
    }

    if (!Valid())
    {
        qDebug() << "Fail during metadata parsing, abort!";
        KvAbort();
    }
    qDebug() << "End load metadata for " << name;
}

void ImageMetadata::InitWithoutMetadata()
{
    qDebug() << "Fail metadata load, try without it";

    sprites_metadata_.clear();

    sprites_metadata_[""].first_frame_pos = 0;

    sprites_metadata_[""].frames_sequence.push_back(0);
    valid_ = true;
}

void ImageMetadata::ParseInfo(const QJsonObject& metadata)
{
    if (metadata.isEmpty())
    {
        qDebug() << "Corrupted metadata!";
        KvAbort();
    }

    QJsonObject info;
    if (!ValidateKey(metadata, "info", &info))
    {
        qDebug() << "Corrupted 'info' key in metadata!";
        KvAbort();
    }
    if (!ValidateKey(info, "version", &version_))
    {
        qDebug() << "Corrupted 'version' key in info!";
        KvAbort();
    }
    if (!ValidateKey(info, "width", &width_))
    {
        qDebug() << "Corrupted 'width' key in info!";
        KvAbort();
    }
    if (!ValidateKey(info, "height", &height_))
    {
        qDebug() << "Corrupted 'height' key in info!";
        KvAbort();
    }

    QJsonArray states;
    if (!ValidateKey(metadata, "states", &states))
    {
        qDebug() << "Corrupted 'states' key in metadata!";
        KvAbort();
    }

    int first_frame_pos = 0;
    for (const QJsonValue& state_value : states)
    {
        QJsonObject state;
        if (!ValidateValue(state_value, &state))
        {
            qDebug() << "Corrupted state in states!" << state_value;
            KvAbort();
        }
        QString state_name;
        if (!ValidateKey(state, "state", &state_name))
        {
            qDebug() << "Corrupted 'state' key in state!";
            KvAbort();
        }
        qDebug() << "State name:" << state_name;

        SpriteMetadata& current_metadata = sprites_metadata_[state_name];
        current_metadata.first_frame_pos = first_frame_pos;

        if (!ValidateKey(state, "dirs", &current_metadata.dirs))
        {
            qDebug() << "Unable to load dirs from state!" << state_name;
            KvAbort();
        }
        int frames_amount;
        if (!ValidateKey(state, "frames", &frames_amount))
        {
            qDebug() << "Unable to load dirs from state!" << state_name;
            KvAbort();
        }
        current_metadata.frames_data.resize(frames_amount);
        first_frame_pos += frames_amount * current_metadata.dirs;

        QJsonArray delays;
        if (ValidateKey(state, "delay", &delays))
        {
            if (delays.size() != current_metadata.frames_data.size())
            {
                qDebug() << "Frames-delays amount mismatch!" << state_name;
                KvAbort();
            }
            for (int i = 0; i < delays.size(); ++i)
            {
                const QJsonValue& value = delays[i];
                double delay;
                if (!ValidateValue(value, &delay))
                {
                    qDebug() << "Corrupted delay value:" << i << state_name;
                    KvAbort();
                }
                current_metadata.frames_data[i].delay = delay;
            }
        }
        else if (current_metadata.frames_data.size() > 1)
        {
            qDebug() << "Delays are missing for frames!" << state_name;
            KvAbort();
        }

        ValidateKey(state, "rewind", &current_metadata.rewind);
        ValidateKey(state, "loop", &current_metadata.loop);

        QJsonArray hotspot;
        if (ValidateKey(state, "hotspot", &hotspot))
        {
            const int HOTSPOT_SIZE = 3;
            if (hotspot.size() != HOTSPOT_SIZE)
            {
                qDebug() << "Invalid hotspot size!" << state_name;
                KvAbort();
            }
            for (int i = 0; i < HOTSPOT_SIZE; ++i)
            {
                const QJsonValue& value = hotspot[i];
                int hotspot_value;
                if (!ValidateValue(value, &hotspot_value))
                {
                    qDebug() << "Corrupted hotspot value:" << state_name;
                    KvAbort();
                }
                current_metadata.hotspot[i] = hotspot_value;
            }
        }
    }

    qDebug() << "Begin make sequence";

    MakeSequence();

    qDebug() << "End make sequence";

    valid_ = true;
}

void ImageMetadata::MakeSequence()
{
    for (auto it = sprites_metadata_.begin(); it != sprites_metadata_.end(); ++it)
    {
        auto& metadata = it->frames_data;
        auto& sequence = it->frames_sequence;
        int local_loop = it->loop;
        if (it->loop == -1 || it->loop == 0)
        {
            local_loop = 1;
        }

        for (int loop_i = 0; loop_i < local_loop; ++loop_i)
        {
            for (quint32 i = 0; i < metadata.size(); ++i)
            {
                sequence.push_back(i);
            }
            if (it->rewind)
            {
                int from = metadata.size() - 2;
                if (from < 0)
                {
                    from = 0;
                }
                for (quint32 i = from; i > 0; --i)
                {
                    sequence.push_back(i);
                }
            }
        }
        if (!(it->loop == -1 || it->loop == 0))
        {
            sequence.push_back(-1);
        }
    }
}
