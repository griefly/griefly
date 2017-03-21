#pragma once

#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QSoundEffect>

#include <QMap>
#include <QVector>

#include <QSharedPointer>

#ifdef PlaySound
#undef PlaySound
#endif // PlaySound

class SoundPlayer
{
public:
    SoundPlayer();
    void PlaySound(const QString& name, float volume = 15.0f);
    void PlayMusic(const QString& name, float volume = 100.0f);
    void StopMusic();
private:
    QMediaPlayer media_player_;
    QMediaPlaylist playlist_;

    using SoundPtr = QSharedPointer<QSoundEffect>;
    QMap<QString, QVector<SoundPtr>> sounds_;
};
