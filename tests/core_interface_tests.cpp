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
}
