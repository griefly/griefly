#pragma once

#include <vector>
#include <string>

#include "FastSerializer.h"

#include "core/Hashes.h"

class ViewInfo;

FastSerializer& operator<<(FastSerializer& file, ViewInfo& view_info);
FastDeserializer& operator>>(FastDeserializer& file, ViewInfo& view_info);

inline unsigned int hash(const ViewInfo& view_info);

class ViewInfo
{
    friend FastSerializer& operator<<(FastSerializer& file, ViewInfo& view_info);
    friend FastDeserializer& operator>>(FastDeserializer& file, ViewInfo& view_info);

    friend unsigned int hash(const ViewInfo& view_info);
public:
    static const int MAX_TRANSPARENCY = 100000;
    // Check if framesets are same
    static bool IsSameFramesets(const ViewInfo& left, const ViewInfo& right);

    class FramesetInfo
    {
        friend FastSerializer& operator<< (
                FastSerializer& file,
                ViewInfo::FramesetInfo& frameset_info);
        friend FastDeserializer& operator>> (
                FastDeserializer& file,
                ViewInfo::FramesetInfo& frameset_info);

        friend unsigned int hash(const ViewInfo::FramesetInfo& frameset);
    public:
        // Check if framesets are basically same
        static bool IsSameSprites(const FramesetInfo& left, const FramesetInfo& right);

        FramesetInfo();

        void SetSprite(const QString& name);
        void SetState(const QString& state);
        void SetAngle(int angle);
        void SetShift(int shift_x, int shift_y);

        const QString& GetState() const { return state_; }
        const QString& GetSprite() const { return sprite_name_; }
        int GetAngle() const { return angle_; }
        int GetShiftX() const { return shift_x_; }
        int GetShiftY() const { return shift_y_; }
    private:
        QString sprite_name_;
        int angle_;
        QString state_;
        int shift_x_;
        int shift_y_;
    };

    ViewInfo();

    FramesetInfo& AddOverlay(const QString& sprite, const QString& state);
    FramesetInfo& AddUnderlay(const QString& sprite, const QString& state);

    void RemoveOverlays();
    void RemoveUnderlays();

    void SetSprite(const QString& sprite) { base_frameset_.SetSprite(sprite); }
    void SetState(const QString& sprite) { base_frameset_.SetState(sprite); }
    void SetAngle(int angle);
    void SetTransparency(int transparency);

    const FramesetInfo& GetBaseFrameset() const { return base_frameset_; }
    int GetAngle() const { return angle_; }
    int GetTransparency() const { return transparency_; }

    const QVector<FramesetInfo>& GetOverlays() const { return overlays_; }
    const QVector<FramesetInfo>& GetUnderlays() const { return underlays_; }
private:
    QVector<FramesetInfo> overlays_;
    QVector<FramesetInfo> underlays_;

    int angle_;
    int transparency_;
    FramesetInfo base_frameset_;
};

inline unsigned int hash(const ViewInfo& view_info)
{
    unsigned int loc = 0;
    for (auto it = view_info.overlays_.begin(); it != view_info.overlays_.end(); ++it)\
    {
        loc += hash(*it);
    }
    for (auto it = view_info.underlays_.begin(); it != view_info.underlays_.end(); ++it)
    {
        loc += hash(*it);
    }
    loc += hash(view_info.base_frameset_);
    loc += hash(view_info.angle_);
    loc += hash(view_info.transparency_);
    return loc;
}

inline unsigned int hash(const ViewInfo::FramesetInfo& frameset_info)
{
    unsigned int loc = 0;
    loc += hash(frameset_info.sprite_name_);
    loc += hash(frameset_info.state_);
    loc += frameset_info.angle_;
    loc += frameset_info.shift_x_;
    loc += frameset_info.shift_y_;
    return loc;
}
