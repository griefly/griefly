#include "representation/ViewInfo.h"

#include <gtest/gtest.h>

TEST(FramesetInfoTest, SettersAndGetters)
{
    ViewInfo::FramesetInfo frameset_info;
    ASSERT_EQ(frameset_info.GetSprite(), "");
    ASSERT_EQ(frameset_info.GetState(), "");
    ASSERT_EQ(frameset_info.GetAngle(), 0);

    frameset_info.SetSprite("something");
    ASSERT_EQ(frameset_info.GetSprite(), "something");
    ASSERT_EQ(frameset_info.GetState(), "");
    ASSERT_EQ(frameset_info.GetAngle(), 0);

    frameset_info.SetState("11112");
    ASSERT_EQ(frameset_info.GetSprite(), "something");
    ASSERT_EQ(frameset_info.GetState(), "11112");
    ASSERT_EQ(frameset_info.GetAngle(), 0);

    frameset_info.SetAngle(42);
    ASSERT_EQ(frameset_info.GetSprite(), "something");
    ASSERT_EQ(frameset_info.GetState(), "11112");
    ASSERT_EQ(frameset_info.GetAngle(), 42);
}

TEST(FramesetInfoTest, IsSameSprites)
{
    ViewInfo::FramesetInfo frameset_info1;
    ASSERT_TRUE(ViewInfo::FramesetInfo::IsSameSprites(frameset_info1, frameset_info1));

    frameset_info1.SetSprite("sprite");
    frameset_info1.SetState("state");
    frameset_info1.SetAngle(42);
    ASSERT_TRUE(ViewInfo::FramesetInfo::IsSameSprites(frameset_info1, frameset_info1));

    ViewInfo::FramesetInfo frameset_info2;
    ASSERT_FALSE(ViewInfo::FramesetInfo::IsSameSprites(frameset_info1, frameset_info2));

    frameset_info2.SetSprite("sprite");
    frameset_info2.SetState("state");
    frameset_info2.SetAngle(42);
    ASSERT_TRUE(ViewInfo::FramesetInfo::IsSameSprites(frameset_info1, frameset_info2));
}
