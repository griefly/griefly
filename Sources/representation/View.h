#pragma once

#include <vector>
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
    class Frameset
    {
        friend std::ostream& operator<<(std::stringstream& file, View::Frameset& frameset);
        friend std::istream& operator>>(std::stringstream& file, View::Frameset& frameset);

        friend unsigned int hash(const View::Frameset& frameset);
    public:
        Frameset();

        bool IsTransp(int x, int y, int shift, int angle);

        void SetSprite(const std::string& name);
        const GLSprite* GetSprite();
        const std::string& GetSpriteName() const { return sprite_name_; }
        void SetState(const std::string& state);
        std::string GetState();
        void SetAngle(int angle);
        int GetAngle() const { return angle_; }
        const ImageMetadata::SpriteMetadata* GetMetadata();
        void Draw(int shift, int x, int y, int angle = 0);
    private:
        std::string sprite_name_;
        const GLSprite* sprite_;

        int angle_;

        std::string state_;
        const ImageMetadata::SpriteMetadata* metadata_;
        int image_state_;
        int last_frame_tick_;
    };
    void SetStepX(int new_step_x)
    {
        step_x_ = new_step_x;
    }
    void SetStepY(int new_step_y)
    {
        step_y_ = new_step_y;
    }
    View();
    
    int GetStepX() const { return step_x_; }
    int GetStepY() const { return step_y_; }

    void AddOverlay(const std::string& sprite, const std::string& state);
    void AddUnderlay(const std::string& sprite, const std::string& state);

    void RemoveOverlays();
    void RemoveUnderlays();

    void SetSprite(const std::string& sprite) { GetBaseFrameset()->SetSprite(sprite); }
    const GLSprite* GetSprite() { return GetBaseFrameset()->GetSprite(); }
    const ImageMetadata::SpriteMetadata* GetMetadata() { return GetBaseFrameset()->GetMetadata(); }

    void SetState(const std::string& sprite) { GetBaseFrameset()->SetState(sprite); }
    void SetAngle(int angle);

    bool IsTransp(int x, int y, int shift);
    void Draw(int shift, int x, int y);
    Frameset* GetBaseFrameset() { return &base_frameset_; }
// Public for test, anyway it View class be deleted soon
public:
    std::vector<Frameset> overlays_;
    std::vector<Frameset> underlays_;

    int angle_;

    int step_x_;
    int step_y_;
    Frameset base_frameset_;
};

std::ostream& operator<<(std::stringstream& file, View::Frameset& frameset);
std::istream& operator>>(std::stringstream& file, View::Frameset& frameset);

inline unsigned int hash(const View::Frameset& frameset);

inline unsigned int hash(const View& view)
{
    unsigned int loc = 0;
    for (auto it = view.overlays_.begin(); it != view.overlays_.end(); ++it)
        loc += hash(*it);
    for (auto it = view.underlays_.begin(); it != view.underlays_.end(); ++it)
        loc += hash(*it);
    loc += hash(view.base_frameset_);
    loc += view.angle_;
    return loc;
}

inline unsigned int hash(const View::Frameset& frameset)
{
    return hash(frameset.sprite_name_) + hash(frameset.state_) + frameset.angle_;
}
