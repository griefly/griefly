#pragma once

#include <vector>
#include <string>

#include "core/FastSerializer.h"
#include "core/Hashes.h"

#include "core_headers/RawViewInfo.h"

class ViewInfo;

kv::FastSerializer& operator<<(kv::FastSerializer& file, const ViewInfo& view_info);
kv::FastDeserializer& operator>>(kv::FastDeserializer& file, ViewInfo& view_info);

inline unsigned int Hash(const ViewInfo& view_info);

const int MAX_TRANSPARENCY = 100000;

namespace kv
{

bool IsSameSprites(
    const kv::RawViewInfo::RawFramesetInfo& left,
    const kv::RawViewInfo::RawFramesetInfo& right);

}

class ViewInfo
{
    friend kv::FastSerializer& operator<<(kv::FastSerializer& file, const ViewInfo& view_info);
    friend kv::FastDeserializer& operator>>(kv::FastDeserializer& file, ViewInfo& view_info);

    friend unsigned int Hash(const ViewInfo& view_info);
public:
    // Check if framesets are same
    static bool IsSameFramesets(const ViewInfo& left, const ViewInfo& right);

    class FramesetInfo
    {
    public:
        static bool IsSameSprites(const FramesetInfo& left, const FramesetInfo& right)
        {
            return kv::IsSameSprites(*left.data_, *right.data_);
        }

        FramesetInfo(kv::RawViewInfo::RawFramesetInfo* data);

        void SetSprite(const QString& name);
        void SetState(const QString& state);
        void SetAngle(int angle);
        void SetShift(int shift_x, int shift_y);

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
    public:
        static bool IsSameSprites(const ConstFramesetInfo& left, const ConstFramesetInfo& right)
        {
            return kv::IsSameSprites(*left.data_, *right.data_);
        }

        // TODO: constructor from FramesetInfo
        ConstFramesetInfo(const kv::RawViewInfo::RawFramesetInfo* data)
            : data_(data)
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
