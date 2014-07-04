#pragma once

#include <map>
#include <string>
#include <vector>

class ImageMetadata
{
public:
    struct SpriteMetadata
    {
        SpriteMetadata()
        {
            dirs = 1;
            rewind = false;
            frames_delay.resize(1);
            frames_delay[0] = 0;
            first_frame_pos = 0;
        }

        size_t dirs;
        std::vector<size_t> frames_delay;
        bool rewind;

        size_t first_frame_pos;
    };
    void Init(const std::string& file_name);
    const SpriteMetadata& GetSpriteMetadata(const std::string& name);
private:
    std::string dmi_version_;
	int width_;
	int height_;

    bool ParseDescription(std::stringstream& desc);
    std::map<std::string, SpriteMetadata> metadata_;
};