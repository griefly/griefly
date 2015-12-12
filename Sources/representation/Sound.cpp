#include "sound.h"

#include "MobPosition.h"
#include "VisiblePoints.h"

SoundManager& GetSoundManager()
{
    static SoundManager snd_mngr;
    return snd_mngr;
}

void InitSound(sf::Sound* sound, const std::string& name)
{
    GetSoundManager().InitSound(sound, name);
}
void PlaySound(sf::Sound* sound, const std::string& name)
{
    if (!sound->getBuffer())
        InitSound(sound, name);
    if (sound->getStatus() != sf::SoundSource::Playing)
        sound->play();
}

SoundPlayer& GetSoundPlayer()
{
    static SoundPlayer snd_player;
    return snd_player;
}

sf::Sound* PlaySound(const std::string& name, int x, int y)
{
    sf::Sound* s = GetSoundPlayer().PlaySound(name);
    //s->setPosition(mob_position::x, y, 0);
    return s;
}

void PlaySoundIfVisible(const std::string& name, size_t tile_id)
{
    if (IsTileVisible(tile_id))
        PlaySound(name);
}
