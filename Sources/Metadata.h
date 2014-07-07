#pragma once

#include <map>
#include <string>
#include <vector>

class ImageMetadata
{
public:
    ImageMetadata() { valid_ = false; }
    struct SpriteMetadata
    {
        SpriteMetadata()
        {
            dirs = 1;
            rewind = false;
            frames_data.resize(1);
            first_frame_pos = 0;
            loop = -1;
        }

        struct FrameMetadata
        {
            FrameMetadata()
            {
                delay = 0;
           /*     state_w = 0;
                state_h = 0; TODO */
            }

            size_t delay;
         /*   size_t state_w;
            size_t state_h;*/
        };
        std::vector<FrameMetadata> frames_data;

        std::vector<int> frames_sequence;
        
        size_t dirs;
        bool rewind;
        int loop;

        size_t first_frame_pos;
    };
    bool Valid() const
    {
        return valid_;
    }
    int GetW() const
    {
        return width_;
    }
    int GetH() const
    {
        return height_;
    }
    void Init(const std::string& file_name);
    const SpriteMetadata& GetSpriteMetadata(const std::string& name);
    bool IsValidState(const std::string& name);
private:
    bool valid_;

    std::string dmi_version_;

    size_t total_width_;
    size_t total_height_;

	size_t width_;
	size_t height_;

    bool ParseDescription(std::stringstream& desc);
    void MakeSequence();

    std::map<std::string, SpriteMetadata> metadata_;
};