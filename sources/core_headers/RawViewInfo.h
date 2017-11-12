#pragma once

#include <tuple>

#include <QVector>

const int MAX_TRANSPARENCY = 100000;

namespace kv
{

struct RawViewInfo
{
public:
    struct RawFramesetInfo
    {
        RawFramesetInfo()
            : angle(0),
              shift_x(0),
              shift_y(0)
        {
            // Nothing
        }

        QString sprite_name;
        int angle;
        QString state;
        int shift_x;
        int shift_y;
    };

    RawViewInfo()
        : angle(0),
          transparency(0)
    {
        // Nothing
    }

    QVector<RawFramesetInfo> overlays;
    QVector<RawFramesetInfo> underlays;

    int angle;
    int transparency;
    RawFramesetInfo base_frameset;
};

inline bool operator==(
    const RawViewInfo::RawFramesetInfo& left,
    const RawViewInfo::RawFramesetInfo& right)
{
    return
           std::tie(left.sprite_name, left.angle, left.state, left.shift_x, left.shift_y)
        == std::tie(right.sprite_name, right.angle, right.state, right.shift_x, right.shift_y);
}

inline bool operator==(
    const RawViewInfo& left,
    const RawViewInfo& right)
{
    return
           std::tie(left.overlays, left.underlays, left.angle, left.transparency, left.base_frameset)
        == std::tie(right.overlays, right.underlays, right.angle, right.transparency, right.base_frameset);
}

}
