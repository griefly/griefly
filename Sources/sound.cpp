#include "sound.h"

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