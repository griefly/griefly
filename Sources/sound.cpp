#include "sound.h"

#include "mob_position.h"

SoundManager& GetSoundManager()
{
    static SoundManager snd_mngr;
    return snd_mngr;
}

void InitSound(sf::Sound* sound, std::string name)
{
    GetSoundManager().InitSound(sound, name);
}
void PlaySound(sf::Sound* sound, std::string name)
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

sf::Sound* PlaySound(std::string name, int x, int y)
{
    sf::Sound* s = GetSoundPlayer().PlaySound(name);
    //s->setPosition(mob_position::x, y, 0);
    return s;
}