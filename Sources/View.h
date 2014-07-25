#pragma once

#include <string>
#include <sstream>

#include "GLSprite.h"
#include "Metadata.h"
#include "hashes.h"

class View;

std::ostream& operator<<(std::stringstream& file, View& view);
std::istream& operator>>(std::stringstream& file, View& view);

inline unsigned int hash(const View& view);

class View
{
    friend std::ostream& operator<<(std::stringstream& file, View& view);
    friend std::istream& operator>>(std::stringstream& file, View& view);

    friend unsigned int hash(const View& view);
public:
    View();
    void SetStepX(int new_step_x)
    {
        step_x_ = new_step_x;
    }
    void SetStepY(int new_step_y)
    {
        step_y_ = new_step_y;
    }
    int GetStepX() const { return step_x_; }
    int GetStepY() const { return step_y_; }

    bool IsTransp(int x, int y, int shift);

    void SetSprite(const std::string& name);
    const GLSprite* GetSprite();
    void SetState(const std::string& state);
    const ImageMetadata::SpriteMetadata* GetMetadata();
    void Draw(int shift, int x, int y);
private:
    int step_x_;
    int step_y_;
    std::string sprite_name_;
    const GLSprite* sprite_;

    std::string state_;
    const ImageMetadata::SpriteMetadata* metadata_;
    int image_state_;
    int last_frame_tick_;
};

inline unsigned int hash(const View& view)
{
    return hash(view.sprite_name_) + hash(view.state_);
}