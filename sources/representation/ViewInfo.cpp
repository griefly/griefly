#include "ViewInfo.h"

kv::FastSerializer& operator<<(kv::FastSerializer& file, const kv::RawViewInfo::RawFramesetInfo& frameset_info)
{
    file << frameset_info.sprite_name;
    file << frameset_info.state;
    file << frameset_info.angle;
    file << frameset_info.shift_x;
    file << frameset_info.shift_y;
    return file;
}

kv::FastDeserializer& operator>>(kv::FastDeserializer& file, kv::RawViewInfo::RawFramesetInfo& frameset_info)
{
    file >> frameset_info.sprite_name;
    file >> frameset_info.state;
    file >> frameset_info.angle;
    file >> frameset_info.shift_x;
    file >> frameset_info.shift_y;
    return file;
}

kv::FastSerializer& operator<<(kv::FastSerializer& file, const ViewInfo& view_info)
{
    file << view_info.data_.base_frameset;

    file << static_cast<quint32>(view_info.data_.underlays.size());
    for (auto it = view_info.data_.underlays.begin(); it != view_info.data_.underlays.end(); ++it)
    {
        file << *it;
    }

    file << static_cast<quint32>(view_info.data_.overlays.size());
    for (auto it = view_info.data_.overlays.begin(); it != view_info.data_.overlays.end(); ++it)
    {
        file << *it;
    }

    file << view_info.data_.angle;
    file << view_info.data_.transparency;

    return file;
}
kv::FastDeserializer& operator>>(kv::FastDeserializer& file, ViewInfo& view_info)
{
    file >> view_info.data_.base_frameset;

    quint32 u_size;
    file >> u_size;
    view_info.data_.underlays.resize(u_size);
    for (quint32 i = 0; i < u_size; ++i)
    {
        kv::RawViewInfo::RawFramesetInfo frameset;
        file >> frameset;
        view_info.data_.underlays[i] = frameset;
    }

    quint32 o_size;
    file >> o_size;
    view_info.data_.overlays.resize(o_size);
    for (quint32 i = 0; i < o_size; ++i)
    {
        kv::RawViewInfo::RawFramesetInfo frameset;
        file >> frameset;
        view_info.data_.overlays[i] = frameset;
    }

    file >> view_info.data_.angle;
    file >> view_info.data_.transparency;

    return file;
}

bool kv::IsSameSprites(
    const kv::RawViewInfo::RawFramesetInfo& left,
    const kv::RawViewInfo::RawFramesetInfo& right)
{
    if (left.sprite_name != right.sprite_name)
    {
        return false;
    }
    if (left.state != right.state)
    {
        return false;
    }
    if (left.angle != right.angle)
    {
        return false;
    }
    if (left.shift_x != right.shift_x)
    {
        return false;
    }
    if (left.shift_y != right.shift_y)
    {
        return false;
    }
    return true;
}

bool ViewInfo::IsSameFramesets(const ViewInfo& left, const ViewInfo& right)
{
    if (left.data_.underlays.size() != right.data_.underlays.size())
    {
        return false;
    }
    if (left.data_.overlays.size() != right.data_.overlays.size())
    {
        return false;
    }

    if (left.data_.angle != right.data_.angle)
    {
        return false;
    }

    if (left.data_.transparency != right.data_.transparency)
    {
        return false;
    }

    if (!kv::IsSameSprites(left.data_.base_frameset, right.data_.base_frameset))
    {
        return false;
    }

    for (int i = 0; i < left.data_.underlays.size(); ++i)
    {
        if (!kv::IsSameSprites(left.data_.underlays[i], right.data_.underlays[i]))
        {
            return false;
        }
    }
    for (int i = 0; i < left.data_.overlays.size(); ++i)
    {
        if (!kv::IsSameSprites(left.data_.overlays[i], right.data_.overlays[i]))
        {
            return false;
        }
    }
    return true;
}

ViewInfo::ViewInfo()
{
    data_.angle = 0;
    data_.transparency = MAX_TRANSPARENCY;
}

void ViewInfo::SetAngle(int angle)
{
    data_.angle = angle;
}

void ViewInfo::SetTransparency(int transparency)
{
    data_.transparency = transparency;
}

ViewInfo::FramesetInfo ViewInfo::AddOverlay(
    const QString& sprite,
    const QString& state)
{
    kv::RawViewInfo::RawFramesetInfo frameset;
    frameset.sprite_name = sprite;
    frameset.state = state;
    data_.overlays.push_back(frameset);
    return ViewInfo::FramesetInfo(&data_.overlays.back());
}
ViewInfo::FramesetInfo ViewInfo::AddUnderlay(
    const QString& sprite,
    const QString& state)
{
    kv::RawViewInfo::RawFramesetInfo frameset;
    frameset.sprite_name = sprite;
    frameset.state = state;
    data_.underlays.push_back(frameset);
    return ViewInfo::FramesetInfo(&data_.underlays.back());
}

void ViewInfo::RemoveOverlays()
{
    data_.overlays.clear();
}
void ViewInfo::RemoveUnderlays()
{
    data_.underlays.clear();
}
