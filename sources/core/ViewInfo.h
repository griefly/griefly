#pragma once

#include <vector>
#include <string>

#include "FastSerializer.h"
#include "Hashes.h"

#include "core_headers/RawViewInfo.h"

inline kv::FastSerializer& operator<<(kv::FastSerializer& file, const kv::RawViewInfo::RawFramesetInfo& frameset_info)
{
    file << frameset_info.sprite_name;
    file << frameset_info.state;
    file << frameset_info.angle;
    file << frameset_info.shift_x;
    file << frameset_info.shift_y;
    return file;
}

inline kv::FastDeserializer& operator>>(kv::FastDeserializer& file, kv::RawViewInfo::RawFramesetInfo& frameset_info)
{
    file >> frameset_info.sprite_name;
    file >> frameset_info.state;
    file >> frameset_info.angle;
    file >> frameset_info.shift_x;
    file >> frameset_info.shift_y;
    return file;
}

class ViewInfo;

kv::FastSerializer& operator<<(kv::FastSerializer& file, const ViewInfo& view_info);
kv::FastDeserializer& operator>>(kv::FastDeserializer& file, ViewInfo& view_info);

inline unsigned int Hash(const ViewInfo& view_info);

class ViewInfo
{
    friend kv::FastSerializer& operator<<(kv::FastSerializer& file, const ViewInfo& view_info);
    friend kv::FastDeserializer& operator>>(kv::FastDeserializer& file, ViewInfo& view_info);

    friend unsigned int Hash(const ViewInfo& view_info);
public:
    class ConstFramesetInfo;
    class FramesetInfo
    {
        friend class ViewInfo::ConstFramesetInfo;
    public:
        FramesetInfo(kv::RawViewInfo::RawFramesetInfo* data)
            : data_(data)
        {
            // Nothing
        }
        void SetSprite(const QString& name)
        {
            data_->sprite_name = name;
        }
        void SetState(const QString& name)
        {
            data_->state = name;
        }
        void SetAngle(int angle)
        {
            data_->angle = angle;
        }
        void SetShift(int shift_x, int shift_y)
        {
            data_->shift_x = shift_x;
            data_->shift_y = shift_y;
        }

        const QString& GetState() const { return data_->state; }
        const QString& GetSprite() const { return data_->sprite_name; }
        int GetAngle() const { return data_->angle; }
        int GetShiftX() const { return data_->shift_x; }
        int GetShiftY() const { return data_->shift_y; }
    private:
        kv::RawViewInfo::RawFramesetInfo* const data_;
    };

    class ConstFramesetInfo
    {
        friend class ViewInfo;
    public:
        ConstFramesetInfo(const kv::RawViewInfo::RawFramesetInfo* data)
            : data_(data)
        {
            // Nothing
        }
        ConstFramesetInfo(const FramesetInfo& other)
            : data_(other.data_)
        {
            // Nothing
        }

        const QString& GetState() const { return data_->state; }
        const QString& GetSprite() const { return data_->sprite_name; }
        int GetAngle() const { return data_->angle; }
        int GetShiftX() const { return data_->shift_x; }
        int GetShiftY() const { return data_->shift_y; }
    private:
        const kv::RawViewInfo::RawFramesetInfo* const data_;
    };

    class FramesetInfoSet
    {
        friend class ViewInfo;
    public:
        int Size() const { return data_->size(); }
        ConstFramesetInfo operator[](int index) const { return ConstFramesetInfo(&(*data_)[index]); }
    private:
        FramesetInfoSet(const QVector<kv::RawViewInfo::RawFramesetInfo>* const data)
            : data_(data)
        {
            // Nothing
        }

        const QVector<kv::RawViewInfo::RawFramesetInfo>* const data_;
    };

    ViewInfo();

    FramesetInfo AddOverlay(const QString& sprite, const QString& state);
    FramesetInfo AddUnderlay(const QString& sprite, const QString& state);

    void RemoveOverlays();
    void RemoveUnderlays();

    void SetSprite(const QString& sprite) { data_.base_frameset.sprite_name = sprite; }
    void SetState(const QString& state) { data_.base_frameset.state = state; }
    void SetAngle(int angle);
    void SetTransparency(int transparency);

    ConstFramesetInfo GetBaseFrameset() const { return ConstFramesetInfo(&data_.base_frameset); }
    int GetAngle() const { return data_.angle; }
    int GetTransparency() const { return data_.transparency; }

    FramesetInfoSet GetOverlays() const { return FramesetInfoSet(&data_.overlays); }
    FramesetInfoSet GetUnderlays() const { return FramesetInfoSet(&data_.underlays); }

    const kv::RawViewInfo& GetRawData() const { return data_; }
private:
    kv::RawViewInfo data_;
};

inline unsigned int Hash(const kv::RawViewInfo::RawFramesetInfo& frameset_info)
{
    unsigned int loc = 0;
    loc += kv::Hash(frameset_info.sprite_name);
    loc += kv::Hash(frameset_info.state);
    loc += frameset_info.angle;
    loc += frameset_info.shift_x;
    loc += frameset_info.shift_y;
    return loc;
}

inline unsigned int Hash(const ViewInfo& view_info)
{
    unsigned int loc = 0;
    for (auto it = view_info.data_.overlays.begin(); it != view_info.data_.overlays.end(); ++it)\
    {
        loc += Hash(*it);
    }
    for (auto it = view_info.data_.underlays.begin(); it != view_info.data_.underlays.end(); ++it)
    {
        loc += Hash(*it);
    }
    loc += Hash(view_info.data_.base_frameset);
    loc += kv::Hash(view_info.data_.angle);
    loc += kv::Hash(view_info.data_.transparency);
    return loc;
}
