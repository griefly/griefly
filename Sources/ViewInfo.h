#pragma once

#include <vector>
#include <string>
#include <sstream>

#include "hashes.h"

class ViewInfo;

std::ostream& operator<<(std::stringstream& file, ViewInfo& view_info);
std::istream& operator>>(std::stringstream& file, ViewInfo& view_info);

inline unsigned int hash(const ViewInfo& view_info);

class ViewInfo
{
    friend std::ostream& operator<<(std::stringstream& file, ViewInfo& view_info);
    friend std::istream& operator>>(std::stringstream& file, ViewInfo& view_info);

    friend unsigned int hash(const ViewInfo& view_info);
public:
    // Check if framesets are same (except angles)
    static bool IsSameFramesets(const ViewInfo& left, const ViewInfo& right);

    class FramesetInfo
    {
        friend std::ostream& operator<< (
                std::stringstream& file,
                ViewInfo::FramesetInfo& frameset_info);
        friend std::istream& operator>> (
                std::stringstream& file,
                ViewInfo::FramesetInfo& frameset_info);

        friend unsigned int hash(const ViewInfo::FramesetInfo& frameset);
    public:
        // Check if framesets are basically same (except angle)
        static bool IsSameSprites(const FramesetInfo& left, const FramesetInfo& right);

        FramesetInfo();

        void SetSprite(const std::string& name);
        void SetState(const std::string& state);
        void SetAngle(int angle);
        const std::string& GetState() const { return state_; }
        const std::string& GetSprite() const { return sprite_name_; }
        int GetAngle() const { return angle_; }
    private:
        std::string sprite_name_;
        int angle_;
        std::string state_;
    };

    ViewInfo();

    void AddOverlay(const std::string& sprite, const std::string& state);
    void AddUnderlay(const std::string& sprite, const std::string& state);

    void RemoveOverlays();
    void RemoveUnderlays();

    void SetSprite(const std::string& sprite) { base_frameset_.SetSprite(sprite); }
    void SetState(const std::string& sprite) { base_frameset_.SetState(sprite); }
    void SetAngle(int angle);

    const FramesetInfo& GetBaseFrameset() const { return base_frameset_; }
    int GetAngle() const { return angle_; }
private:
    std::vector<FramesetInfo> overlays_;
    std::vector<FramesetInfo> underlays_;

    int angle_;
    FramesetInfo base_frameset_;
};

inline unsigned int hash(const ViewInfo& view_info)
{
    unsigned int loc = 0;
    for (auto it = view_info.overlays_.begin(); it != view_info.overlays_.end(); ++it)
        loc += hash(*it);
    for (auto it = view_info.underlays_.begin(); it != view_info.underlays_.end(); ++it)
        loc += hash(*it);
    loc += hash(view_info.base_frameset_);
    loc += view_info.angle_;
    return loc;
}

inline unsigned int hash(const ViewInfo::FramesetInfo& frameset_info)
{
    return hash(frameset_info.sprite_name_) + hash(frameset_info.state_) + frameset_info.angle_;
}
