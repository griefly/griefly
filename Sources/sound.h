#include <string>
#include <hash_map>
#include <vector>

#include <SFML/Audio.hpp>

#include "constheader.h"

class SoundManager
{
public:
    void InitSound(sf::Sound* sound, std::string name)
    {
        sound->setBuffer(*GetBuffer(name));
    }
private:
    const sf::SoundBuffer* GetBuffer(std::string name)
    {
        if (!holder_[name])
        {
            SYSTEM_STREAM << "Load sound " << name << std::endl;
            holder_[name] = new sf::SoundBuffer;
            if (!holder_[name]->loadFromFile("sounds/" + name))
                SYSTEM_STREAM << "Error during load sound " << name << std::endl;      
        }
        return holder_[name];
    }
    std::hash_map<std::string, sf::SoundBuffer*> holder_;
};

SoundManager& GetSoundManager();
void InitSound(sf::Sound* sound, std::string name);
void PlaySound(sf::Sound* sound, std::string name);

sf::Sound* PlaySound(std::string name, int x = 0, int y = 0);

class SoundPlayer
{
public:
    SoundPlayer()
    {
        sounds_.resize(100);
    }
    sf::Sound* PlaySound(std::string name)
    {
        size_t i;
        for (i = 0; i < sounds_.size(); ++i)
            if (sounds_[i].getStatus() == sf::SoundSource::Stopped)
                break;
        if (i == sounds_.size())
            sounds_.resize(sounds_.size() * 2);
        GetSoundManager().InitSound(&sounds_[i], name);
        sounds_[i].play();
        return &sounds_[i];
    }
private:
    std::vector<sf::Sound> sounds_;
};

SoundPlayer& GetSoundPlayer();