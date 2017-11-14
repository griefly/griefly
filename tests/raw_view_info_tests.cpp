#include "core_headers/RawViewInfo.h"

#include <gtest/gtest.h>

TEST(RawFramesetInfo, OperatorEqual)
{
    kv::RawViewInfo::RawFramesetInfo info;
    EXPECT_EQ(info, info);

    info.angle = 2;
    info.shift_x = -10;
    info.shift_y = 22;
    info.sprite_name = "name";
    info.state = "long";
    EXPECT_EQ(info, info);

    kv::RawViewInfo::RawFramesetInfo info2 = info;
    EXPECT_EQ(info, info2);
    info2.angle = 3;
    EXPECT_NE(info, info2);
    info2.angle = 2;
    info2.shift_x = 3;
    EXPECT_NE(info, info2);
    info2.shift_x = -10;
    info2.shift_y = -10;
    EXPECT_NE(info, info2);
    info2.shift_y = 22;
    info2.sprite_name = "not name";
    EXPECT_NE(info, info2);
    info2.sprite_name = "name";
    info2.state = "111";
    EXPECT_NE(info, info2);
    info2.state = "long";
    EXPECT_EQ(info, info2);
}
