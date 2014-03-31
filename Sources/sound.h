#include <string>
#include <map>

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
    std::map<std::string, sf::SoundBuffer*> holder_;
};

SoundManager& GetSoundManager();
void InitSound(sf::Sound* sound, std::string name);
void PlaySound(sf::Sound* sound, std::string name);