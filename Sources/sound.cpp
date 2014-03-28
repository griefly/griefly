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