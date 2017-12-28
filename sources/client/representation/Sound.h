#pragma once

#include <memory>

#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QSoundEffect>

#include <QMap>
#include <QVector>

#ifdef PlaySound
#undef PlaySound
#endif // PlaySound

class SoundPlayer
{
public:
    SoundPlayer();
    void PlaySound(const QString& name, float volume = 15.0f);
    void PlayMusic(const QString& name, int volume = 100);
    void StopMusic();
private:
    QMediaPlayer media_player_;
    QMediaPlaylist playlist_;

    using SoundPtr = std::shared_ptr<QSoundEffect>;
    QMap<QString, QVector<SoundPtr>> sounds_;
};
