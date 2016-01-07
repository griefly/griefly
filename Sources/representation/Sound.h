#include <string>
#include <unordered_map>
#include <vector>

#include <SFML/Audio.hpp>

#include "core/constheader.h"

class SoundManager
{
public:
    void InitSound(sf::Sound* sound, const std::string& name);
    void LoadSound(const std::string& name);
private:
    const sf::SoundBuffer* GetBuffer(const std::string& name);
    std::unordered_map<std::string, sf::SoundBuffer*> holder_;
};

SoundManager& GetSoundManager();

void PlaySoundIfVisible(const std::string& name, size_t tile_id);
void PlayMusic(const std::string& name, float volume = 100.0f);

class SoundPlayer
{
public:
    SoundPlayer();
    sf::Sound* PlaySound(const std::string& name);
    void PlayMusic(const std::string& name, float volume = 100.0f);
    void StopMusic();
    bool IsMusicPlaying();
private:
    sf::Music music_;
    std::vector<sf::Sound> sounds_;
};

SoundPlayer& GetSoundPlayer();
