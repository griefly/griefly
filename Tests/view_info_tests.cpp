#include "representation/ViewInfo.h"

#include <gtest/gtest.h>

TEST(FramesetInfo, SettersAndGetters)
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

TEST(FramesetInfo, IsSameSprites)
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

    frameset_info2.SetSprite("sprite2");
    ASSERT_FALSE(ViewInfo::FramesetInfo::IsSameSprites(frameset_info1, frameset_info2));

    frameset_info2.SetSprite("sprite");
    frameset_info2.SetState("state2");
    ASSERT_FALSE(ViewInfo::FramesetInfo::IsSameSprites(frameset_info1, frameset_info2));

    frameset_info2.SetState("state");
    frameset_info2.SetAngle(43);
    ASSERT_FALSE(ViewInfo::FramesetInfo::IsSameSprites(frameset_info1, frameset_info2));
}

TEST(FramesetInfo, StreamOperators)
{
    ViewInfo::FramesetInfo frameset_info;
    frameset_info.SetSprite("sprite 1");
    frameset_info.SetState("state 2");
    frameset_info.SetAngle(84);

    std::stringstream str;
    str << frameset_info;

    ASSERT_EQ(str.str(), "8 sprite 1 7 state 2 84 ");

    str.str("8 sprite 2 7 state 3 -14");

    ViewInfo::FramesetInfo frameset_info2;
    str >> frameset_info2;
    ASSERT_EQ(frameset_info2.GetSprite(), "sprite 2");
    ASSERT_EQ(frameset_info2.GetState(), "state 3");
    ASSERT_EQ(frameset_info2.GetAngle(), -14);

    std::stringstream str2;
    str2 << frameset_info;
    ViewInfo::FramesetInfo frameset_info3;
    str2 >> frameset_info3;
    ASSERT_TRUE(ViewInfo::FramesetInfo::IsSameSprites(frameset_info, frameset_info3));
}

TEST(FramesetInfo, Hash)
{
    ViewInfo::FramesetInfo frameset_info;
    ASSERT_EQ(hash(frameset_info), 2);

    frameset_info.SetSprite("sprite 1");
    frameset_info.SetState("state 2");
    frameset_info.SetAngle(84);
    ASSERT_EQ(hash(frameset_info), 230996884);
}

TEST(ViewInfo, AngleAndBaseFrameset)
{
    ViewInfo view_info;
    ASSERT_EQ(view_info.GetAngle(), 0);

    view_info.SetSprite("windows better than linux");
    ASSERT_EQ(view_info.GetBaseFrameset().GetSprite(), "windows better than linux");
    ASSERT_EQ(view_info.GetBaseFrameset().GetState(), "");
    ASSERT_EQ(view_info.GetAngle(), 0);

    view_info.SetState("lol jk");
    ASSERT_EQ(view_info.GetBaseFrameset().GetSprite(), "windows better than linux");
    ASSERT_EQ(view_info.GetBaseFrameset().GetState(), "lol jk");
    ASSERT_EQ(view_info.GetAngle(), 0);

    view_info.SetAngle(33);
    ASSERT_EQ(view_info.GetBaseFrameset().GetSprite(), "windows better than linux");
    ASSERT_EQ(view_info.GetBaseFrameset().GetState(), "lol jk");
    ASSERT_EQ(view_info.GetAngle(), 33);
}

TEST(ViewInfo, OverlaysAndUnderlays)
{
    ViewInfo view_info;
    ASSERT_EQ(view_info.GetOverlays().size(), 0);
    ASSERT_EQ(view_info.GetUnderlays().size(), 0);

    view_info.AddOverlay("bom bom", "bim bom");
    ASSERT_EQ(view_info.GetOverlays().size(), 1);
    ASSERT_EQ(view_info.GetUnderlays().size(), 0);
    ASSERT_EQ(view_info.GetOverlays()[0].GetSprite(), "bom bom");
    ASSERT_EQ(view_info.GetOverlays()[0].GetState(), "bim bom");

    view_info.AddOverlay("bom bom2", "bim bom2");
    ASSERT_EQ(view_info.GetOverlays().size(), 2);
    ASSERT_EQ(view_info.GetUnderlays().size(), 0);
    ASSERT_EQ(view_info.GetOverlays()[1].GetSprite(), "bom bom2");
    ASSERT_EQ(view_info.GetOverlays()[1].GetState(), "bim bom2");

    view_info.AddUnderlay("image", "state");
    ASSERT_EQ(view_info.GetOverlays().size(), 2);
    ASSERT_EQ(view_info.GetUnderlays().size(), 1);
    ASSERT_EQ(view_info.GetUnderlays()[0].GetSprite(), "image");
    ASSERT_EQ(view_info.GetUnderlays()[0].GetState(), "state");

    view_info.AddUnderlay("image2", "state2");
    ASSERT_EQ(view_info.GetOverlays().size(), 2);
    ASSERT_EQ(view_info.GetUnderlays().size(), 2);
    ASSERT_EQ(view_info.GetUnderlays()[1].GetSprite(), "image2");
    ASSERT_EQ(view_info.GetUnderlays()[1].GetState(), "state2");

    view_info.RemoveOverlays();
    ASSERT_EQ(view_info.GetOverlays().size(), 0);
    ASSERT_EQ(view_info.GetUnderlays().size(), 2);

    view_info.RemoveUnderlays();
    ASSERT_EQ(view_info.GetOverlays().size(), 0);
    ASSERT_EQ(view_info.GetUnderlays().size(), 0);
}






