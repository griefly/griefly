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

TEST(RawViewInfo, OperatorEqual)
{
    kv::RawViewInfo info;
    EXPECT_EQ(info, info);

    info.angle = 3;
    info.transparency = 15;
    info.overlays.resize(2);
    info.underlays.resize(3);
    info.base_frameset.sprite_name = "testing";
    EXPECT_EQ(info, info);

    kv::RawViewInfo info2 = info;
    EXPECT_EQ(info2, info);
    info2.angle = 4;
    EXPECT_NE(info2, info);
    info2.angle = 3;
    info2.transparency = 200;
    EXPECT_NE(info2, info);
    info2.transparency = 15;
    info2.overlays.resize(4);
    EXPECT_NE(info2, info);
    info2.overlays.resize(2);
    info.underlays.resize(4);
    EXPECT_NE(info2, info);
    info.underlays.resize(3);
    info.base_frameset.angle = 3;
    EXPECT_NE(info2, info);
    info.base_frameset.angle = 0;
    EXPECT_EQ(info2, info);
}
