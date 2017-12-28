#pragma once

#include <qglobal.h>

#include <QMap>
#include <QString>
#include <QVector>
#include <QJsonObject>

class ImageMetadata
{
public:
    ImageMetadata() { valid_ = false; }
    struct SpriteMetadata
    {
        SpriteMetadata()
        {
            dirs = 1;
            rewind = 0;
            frames_data.resize(1);
            first_frame_pos = 0;
            loop = -1;
            hotspot[0] = -1;
            hotspot[1] = -1;
            hotspot[2] = -1;
        }

        struct FrameMetadata
        {
            FrameMetadata()
            {
                delay = 0.0;
            }
            double delay;
        };
        QVector<FrameMetadata> frames_data;

        QVector<int> frames_sequence;
        
        int dirs;
        int rewind;
        int loop;
        int hotspot[3];

        quint32 first_frame_pos;
    };
    bool Valid() const
    {
        return valid_;
    }
    int GetW() const
    {
        return width_;
    }
    int GetH() const
    {
        return height_;
    }
    void Init(const QString& file_name, int width, int height);

    const SpriteMetadata& GetSpriteMetadata(const QString& name) const;
    bool IsValidState(const QString& name) const;
private:
    void InitWithoutMetadata();

    bool valid_;

    double version_;

    int width_;
    int height_;

    void ParseInfo(const QJsonObject& metadata);
    void MakeSequence();

    QMap<QString, SpriteMetadata> sprites_metadata_;
};
