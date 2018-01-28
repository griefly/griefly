#include "Sound.h"

#include <QDebug>
#include <QFileInfo>

#include <QSound>

SoundPlayer::SoundPlayer()
{
    // Do nothing
}

void SoundPlayer::PlaySound(const QString& name, float volume)
{
    QVector<SoundPtr>& sound_cache = sounds_[name];

    SoundPtr ptr;

    for (auto it = sound_cache.begin(); it != sound_cache.end(); ++it)
    {
        if (!(*it)->isPlaying())
        {
            ptr = *it;
            break;
        }
    }

    if (!ptr)
    {
        const int MAXIMUM_SOUND_CACHE_SIZE = 8;
        if (sound_cache.size() >= MAXIMUM_SOUND_CACHE_SIZE)
        {
            return;
        }
        ptr.reset(new QSoundEffect, [](QSoundEffect* effect) { effect->deleteLater(); });
        ptr->setSource(QUrl::fromLocalFile("sounds/" + name));
        sound_cache.append(ptr);
    }

    ptr->setVolume(static_cast<double>(volume) / 100.0);
    ptr->play();
}

void SoundPlayer::PlayMusic(const QString &name, int volume)
{
    StopMusic();

    playlist_.clear();

    // Seems like QMediaPlayer cannot handle relative paths
    QFileInfo file_info("music/" + name);
    qDebug() << file_info.absoluteFilePath();
    if (!playlist_.addMedia(QUrl::fromLocalFile(file_info.absoluteFilePath())))
    {
        qDebug() << "Cannot add music file to playlist: " << "music/" << name;
        return;
    }
    playlist_.setPlaybackMode(QMediaPlaylist::Loop);

    media_player_.setPlaylist(&playlist_);
    media_player_.setVolume(static_cast<int>(volume));
    media_player_.play();
}

void SoundPlayer::StopMusic()
{
    media_player_.stop();
}
