#pragma once

#include <QVector>

namespace kv
{

struct RawViewInfo
{
public:
    struct RawFramesetInfo
    {
        QString sprite_name_;
        int angle_;
        QString state_;
        int shift_x_;
        int shift_y_;
    };

    QVector<RawFramesetInfo> overlays_;
    QVector<RawFramesetInfo> underlays_;

    int angle_;
    int transparency_;
    RawFramesetInfo base_frameset_;
};

}
