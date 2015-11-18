#include "ViewInfo.h"

std::ostream& operator<<(std::stringstream& file, ViewInfo& view_info)
{
    WrapWriteMessage(file, *view_info.GetBaseFrameset());
    file << " ";

    file << view_info.underlays_.size();
    file << " ";
    for (auto it = view_info.underlays_.begin(); it != view_info.underlays_.end(); ++it)
    {
            WrapWriteMessage(file, *it);
            file << " ";
    }

    file << view_info.overlays_.size();
    file << " ";
    for (auto it = view_info.overlays_.begin(); it != view_info.overlays_.end(); ++it)
    {
            WrapWriteMessage(file, *it);
            file << " ";
    }

    file << view_info.angle_ << " ";

    return file;
}
std::istream& operator>>(std::stringstream& file, ViewInfo& view_info)
{
    WrapReadMessage(file, *view_info.GetBaseFrameset());

    size_t u_size;
    file >> u_size;
    view_info.underlays_.resize(u_size);
    for (size_t i = 0; i < u_size; ++i)
    {
        ViewInfo::FramesetInfo f;
        WrapReadMessage(file, f);
        view_info.underlays_[i] = f;
    }

    size_t o_size;
    file >> o_size;
    view_info.overlays_.resize(o_size);
    for (size_t i = 0; i < o_size; ++i)
    {
        ViewInfo::FramesetInfo f;
        WrapReadMessage(file, f);
        view_info.overlays_[i] = f;
    }

    file >> view_info.angle_;

    return file;
}

std::ostream& operator<<(std::stringstream& file, ViewInfo::FramesetInfo& frameset_info)
{
    WrapWriteMessage(file, frameset_info.sprite_name_);
    file << " ";
    WrapWriteMessage(file, frameset_info.state_);
    file << " ";
    file << frameset_info.angle_;
    file << " ";
    return file;
}
std::istream& operator>>(std::stringstream& file, ViewInfo::FramesetInfo& frameset_info)
{
    WrapReadMessage(file, frameset_info.sprite_name_);
    WrapReadMessage(file, frameset_info.state_);
    file >> frameset_info.angle_;
    return file;
}

ViewInfo::FramesetInfo::FramesetInfo()
{
    sprite_name_ = "";
    state_ = "";
    angle_ = 0;
}

void ViewInfo::FramesetInfo::SetAngle(int angle)
{
    angle_ = angle;
}

void ViewInfo::FramesetInfo::SetSprite(const std::string& name)
{
    sprite_name_ = name;
}

std::string ViewInfo::FramesetInfo::GetState()
{
    return state_;
}

void ViewInfo::FramesetInfo::SetState(const std::string& name)
{
    state_ = name;
}
