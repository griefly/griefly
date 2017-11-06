#pragma once

#include <QVector>

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

}
