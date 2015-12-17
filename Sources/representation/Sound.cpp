#include "Sound.h"

#include "MobPosition.h"
#include "VisiblePoints.h"

SoundManager& GetSoundManager()
{
    static SoundManager snd_mngr;
    return snd_mngr;
}

void SoundManager::InitSound(sf::Sound* sound, const std::string& name)
{
    sound->setBuffer(*GetBuffer(name));
}

void SoundManager::LoadSound(const std::string& name)
{
    SYSTEM_STREAM << "Load sound " << name << std::endl;
    holder_[name] = new sf::SoundBuffer;
    if (!holder_[name]->loadFromFile("sounds/" + name))
    {
        SYSTEM_STREAM << "Error during load sound " << name << std::endl;
    }
}

const sf::SoundBuffer* SoundManager::GetBuffer(const std::string& name)
{
    if (!holder_[name])
    {
        LoadSound(name);
    }
    return holder_[name];
}

void InitSound(sf::Sound* sound, const std::string& name)
{
    GetSoundManager().InitSound(sound, name);
}
void PlaySound(sf::Sound* sound, const std::string& name)
{
    if (!sound->getBuffer())
    {
        InitSound(sound, name);
    }
    if (sound->getStatus() != sf::SoundSource::Playing)
    {
        sound->play();
    }
}

SoundPlayer& GetSoundPlayer()
{
    static SoundPlayer snd_player;
    return snd_player;
}

SoundPlayer::SoundPlayer()
{
    sounds_.resize(100);
}

sf::Sound* SoundPlayer::PlaySound(const std::string& name)
{
    // TODO: 256 limit in sfml
    size_t i;
    for (i = 0; i < sounds_.size(); ++i)
    {
        if (sounds_[i].getStatus() == sf::SoundSource::Stopped)
        {
            break;
        }
    }
    if (i == sounds_.size())
    {
        sounds_.resize(sounds_.size() * 2);
    }
    GetSoundManager().InitSound(&sounds_[i], name);
    // TODO: volume
    sounds_[i].setVolume(15.0f);
    sounds_[i].play();
    return &sounds_[i];
}

void SoundPlayer::PlayMusic(const std::string &name, float volume)
{
    StopMusic();

    if (!music_.openFromFile("music/" + name))
    {
        SYSTEM_STREAM << "Cannot open music file " << "music/" << name << std::endl;
    }
    music_.setLoop(true);
    music_.setVolume(volume);
    music_.play();
}

void SoundPlayer::StopMusic()
{
    if (music_.getStatus() != sf::Music::Status::Stopped)
    {
        music_.stop();
    }
}

sf::Sound* PlaySound(const std::string& name)
{
    sf::Sound* s = GetSoundPlayer().PlaySound(name);
    //s->setPosition(mob_position::x, y, 0);
    return s;
}

void PlaySoundIfVisible(const std::string& name, size_t tile_id)
{
    if (IsTileVisible(tile_id))
    {
        PlaySound(name);
    }
}


void PlayMusic(const std::string& name, float volume)
{
    if (name == "")
    {
        GetSoundPlayer().StopMusic();
        return;
    }
    GetSoundPlayer().PlayMusic(name, volume);
}
