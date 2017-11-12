#include "ViewInfo.h"

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
