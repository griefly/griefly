#include "representation/ViewInfo.h"

#include <gtest/gtest.h>

TEST(FramesetInfoTest, SettersAndGetters)
{
    ViewInfo::FramesetInfo frameset_info_;
    ASSERT_EQ(frameset_info_.GetSprite(), "");
    ASSERT_EQ(frameset_info_.GetState(), "");
    ASSERT_EQ(frameset_info_.GetAngle(), 0);

    frameset_info_.SetSprite("something");
    ASSERT_EQ(frameset_info_.GetSprite(), "something");
    ASSERT_EQ(frameset_info_.GetState(), "");
    ASSERT_EQ(frameset_info_.GetAngle(), 0);

    frameset_info_.SetState("11112");
    ASSERT_EQ(frameset_info_.GetSprite(), "something");
    ASSERT_EQ(frameset_info_.GetState(), "11112");
    ASSERT_EQ(frameset_info_.GetAngle(), 0);

    frameset_info_.SetAngle(42);
    ASSERT_EQ(frameset_info_.GetSprite(), "something");
    ASSERT_EQ(frameset_info_.GetState(), "11112");
    ASSERT_EQ(frameset_info_.GetAngle(), 42);
}
