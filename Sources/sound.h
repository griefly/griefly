#include <string>
#include <unordered_map>
#include <vector>

#include <SFML/Audio.hpp>

#include "constheader.h"

class SoundManager
{
public:
    void InitSound(sf::Sound* sound, const std::string& name)
    {
        sound->setBuffer(*GetBuffer(name));
    }
    void LoadSound(const std::string& name)
    {
        SYSTEM_STREAM << "Load sound " << name << std::endl;
        holder_[name] = new sf::SoundBuffer;
        if (!holder_[name]->loadFromFile("sounds/" + name))
            SYSTEM_STREAM << "Error during load sound " << name << std::endl;    
    }
private:
    const sf::SoundBuffer* GetBuffer(const std::string& name)
    {
        if (!holder_[name])
            LoadSound(name);    
        return holder_[name];
    }
    std::unordered_map<std::string, sf::SoundBuffer*> holder_;
};

SoundManager& GetSoundManager();
void InitSound(sf::Sound* sound, const std::string& name);
void PlaySound(sf::Sound* sound, const std::string& name);

sf::Sound* PlaySound(const std::string& name, int x = 0, int y = 0);

void PlaySoundIfVisible(const std::string& name, size_t tile_id);

class SoundPlayer
{
public:
    SoundPlayer()
    {
        sounds_.resize(100);
    }
    sf::Sound* PlaySound(const std::string& name)
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
