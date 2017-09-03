#include "CoreInterface.h"

#include <gtest/gtest.h>

TEST(CoreInterface, FrameDataAndNestedStructsConstructors)
{
    kv::FrameData::Entity entity;
    EXPECT_EQ(entity.id, 0);
    EXPECT_EQ(entity.click_id, 0);
    EXPECT_EQ(entity.pos_x, 0);
    EXPECT_EQ(entity.pos_y, 0);
    EXPECT_EQ(entity.vlevel, 0);
    EXPECT_EQ(entity.dir, Dir::SOUTH);

    kv::FrameData::InterfaceUnit unit;
    EXPECT_EQ(unit.pixel_x, 0);
    EXPECT_EQ(unit.pixel_y, 0);
    EXPECT_EQ(unit.shift, 0);

    kv::FrameData::Music music;
    EXPECT_EQ(music.volume, 0);

    kv::FrameData frame;
    EXPECT_EQ(frame.camera_pos_x, 0);
    EXPECT_EQ(frame.camera_pos_y, 0);
}
