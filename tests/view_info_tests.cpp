#include "representation/ViewInfo.h"

#include <gtest/gtest.h>

TEST(FramesetInfo, SettersAndGetters)
{
    kv::RawViewInfo::RawFramesetInfo raw_info;

    ViewInfo::FramesetInfo frameset_info(&raw_info);
    ASSERT_EQ(frameset_info.GetSprite(), "");
    ASSERT_EQ(frameset_info.GetState(), "");
    ASSERT_EQ(frameset_info.GetAngle(), 0);
    ASSERT_EQ(frameset_info.GetShiftX(), 0);
    ASSERT_EQ(frameset_info.GetShiftY(), 0);

    frameset_info.SetSprite("something");
    ASSERT_EQ(frameset_info.GetSprite(), "something");
    ASSERT_EQ(frameset_info.GetState(), "");
    ASSERT_EQ(frameset_info.GetAngle(), 0);
    ASSERT_EQ(frameset_info.GetShiftX(), 0);
    ASSERT_EQ(frameset_info.GetShiftY(), 0);

    frameset_info.SetState("11112");
    ASSERT_EQ(frameset_info.GetSprite(), "something");
    ASSERT_EQ(frameset_info.GetState(), "11112");
    ASSERT_EQ(frameset_info.GetAngle(), 0);
    ASSERT_EQ(frameset_info.GetShiftX(), 0);
    ASSERT_EQ(frameset_info.GetShiftY(), 0);

    frameset_info.SetAngle(42);
    ASSERT_EQ(frameset_info.GetSprite(), "something");
    ASSERT_EQ(frameset_info.GetState(), "11112");
    ASSERT_EQ(frameset_info.GetAngle(), 42);
    ASSERT_EQ(frameset_info.GetShiftX(), 0);
    ASSERT_EQ(frameset_info.GetShiftY(), 0);

    frameset_info.SetShift(11, 22);
    ASSERT_EQ(frameset_info.GetSprite(), "something");
    ASSERT_EQ(frameset_info.GetState(), "11112");
    ASSERT_EQ(frameset_info.GetAngle(), 42);
    ASSERT_EQ(frameset_info.GetShiftX(), 11);
    ASSERT_EQ(frameset_info.GetShiftY(), 22);
}

TEST(FramesetInfo, IsSameSprites)
{
    kv::RawViewInfo::RawFramesetInfo raw_info1;
    ViewInfo::FramesetInfo frameset_info1(&raw_info1);
    EXPECT_TRUE(ViewInfo::IsSameSprites(frameset_info1, frameset_info1));

    frameset_info1.SetSprite("sprite");
    frameset_info1.SetState("state");
    frameset_info1.SetAngle(42);
    frameset_info1.SetShift(420, 420);
    EXPECT_TRUE(ViewInfo::IsSameSprites(frameset_info1, frameset_info1));

    kv::RawViewInfo::RawFramesetInfo raw_info2;
    ViewInfo::FramesetInfo frameset_info2(&raw_info2);
    EXPECT_FALSE(ViewInfo::IsSameSprites(frameset_info1, frameset_info2));

    frameset_info2.SetSprite("sprite");
    frameset_info2.SetState("state");
    frameset_info2.SetAngle(42);
    frameset_info2.SetShift(420, 420);
    EXPECT_TRUE(ViewInfo::IsSameSprites(frameset_info1, frameset_info2));

    frameset_info2.SetSprite("sprite2");
    EXPECT_FALSE(ViewInfo::IsSameSprites(frameset_info1, frameset_info2));

    frameset_info2.SetSprite("sprite");
    frameset_info2.SetState("state2");
    EXPECT_FALSE(ViewInfo::IsSameSprites(frameset_info1, frameset_info2));

    frameset_info2.SetState("state");
    frameset_info2.SetAngle(43);
    EXPECT_FALSE(ViewInfo::IsSameSprites(frameset_info1, frameset_info2));

    frameset_info2.SetAngle(42);
    frameset_info2.SetShift(1, 420);
    EXPECT_FALSE(ViewInfo::IsSameSprites(frameset_info1, frameset_info2));

    frameset_info2.SetShift(420, 1);
    EXPECT_FALSE(ViewInfo::IsSameSprites(frameset_info1, frameset_info2));
}

// TODO: ConstFrameInfo tests
// TODO: RawFramesetInfo
/*TEST(FramesetInfo, StreamOperators)
{
    kv::RawViewInfo::RawFramesetInfo raw_info;
    ViewInfo::FramesetInfo frameset_info(&raw_info);
    frameset_info.SetSprite("sprite 1");
    frameset_info.SetState("state 2");
    frameset_info.SetAngle(84);
    frameset_info.SetShift(12, 13);

    kv::FastSerializer serialzier(1);
    serialzier << frameset_info;

    kv::FastDeserializer deserializer(serialzier.GetData(), serialzier.GetIndex());

    kv::RawViewInfo::RawFramesetInfo raw_info2;
    ViewInfo::FramesetInfo frameset_info2(&raw_info2);
    deserializer >> frameset_info2;
    EXPECT_EQ(frameset_info2.GetSprite(), "sprite 1");
    EXPECT_EQ(frameset_info2.GetState(), "state 2");
    EXPECT_EQ(frameset_info2.GetAngle(), 84);
    EXPECT_EQ(frameset_info2.GetShiftX(), 12);
    EXPECT_EQ(frameset_info2.GetShiftY(), 13);

    {
        kv::FastSerializer serializer(1);
        serializer << frameset_info;
        kv::FastDeserializer deserializer(serializer.GetData(), serializer.GetIndex());
        kv::RawViewInfo::RawFramesetInfo raw_info3;
        ViewInfo::FramesetInfo frameset_info3(&raw_info3);
        deserializer >> frameset_info3;
        EXPECT_TRUE(ViewInfo::FramesetInfo::IsSameSprites(frameset_info, frameset_info3));
    }
}*/

TEST(FramesetInfo, Hash)
{
    kv::RawViewInfo::RawFramesetInfo frameset_info;
    ASSERT_EQ(Hash(frameset_info), 2);

    frameset_info.sprite_name = "sprite 1";
    ASSERT_EQ(Hash(frameset_info), 2014229692);

    frameset_info.state = "state 2";
    ASSERT_EQ(Hash(frameset_info), 230996800);

    frameset_info.angle = 84;
    ASSERT_EQ(Hash(frameset_info), 230996884);

    frameset_info.shift_x = 0;
    frameset_info.shift_y = 13;
    ASSERT_EQ(Hash(frameset_info), 230996897);

    frameset_info.shift_x = 14;
    frameset_info.shift_y = 13;
    ASSERT_EQ(Hash(frameset_info), 230996911);
}

TEST(ViewInfo, AngleAndBaseFrameset)
{
    ViewInfo view_info;
    EXPECT_EQ(view_info.GetAngle(), 0);
    EXPECT_EQ(view_info.GetTransparency(), MAX_TRANSPARENCY);

    view_info.SetSprite("windows better than linux");
    EXPECT_EQ(view_info.GetBaseFrameset().GetSprite(), "windows better than linux");
    EXPECT_EQ(view_info.GetBaseFrameset().GetState(), "");
    EXPECT_EQ(view_info.GetAngle(), 0);
    EXPECT_EQ(view_info.GetTransparency(), MAX_TRANSPARENCY);

    view_info.SetState("lol jk");
    EXPECT_EQ(view_info.GetBaseFrameset().GetSprite(), "windows better than linux");
    EXPECT_EQ(view_info.GetBaseFrameset().GetState(), "lol jk");
    EXPECT_EQ(view_info.GetAngle(), 0);
    EXPECT_EQ(view_info.GetTransparency(), MAX_TRANSPARENCY);

    view_info.SetAngle(33);
    EXPECT_EQ(view_info.GetBaseFrameset().GetSprite(), "windows better than linux");
    EXPECT_EQ(view_info.GetBaseFrameset().GetState(), "lol jk");
    EXPECT_EQ(view_info.GetAngle(), 33);
    EXPECT_EQ(view_info.GetTransparency(), MAX_TRANSPARENCY);

    view_info.SetTransparency(9876);
    EXPECT_EQ(view_info.GetBaseFrameset().GetSprite(), "windows better than linux");
    EXPECT_EQ(view_info.GetBaseFrameset().GetState(), "lol jk");
    EXPECT_EQ(view_info.GetAngle(), 33);
    EXPECT_EQ(view_info.GetTransparency(), 9876);
}

TEST(ViewInfo, OverlaysAndUnderlays)
{
    ViewInfo view_info;
    ASSERT_EQ(view_info.GetOverlays().Size(), 0);
    ASSERT_EQ(view_info.GetUnderlays().Size(), 0);

    {
        ViewInfo::FramesetInfo overlay
            = view_info.AddOverlay("bom bom", "bim bom");
        ASSERT_EQ(view_info.GetOverlays().Size(), 1);
        ASSERT_EQ(view_info.GetUnderlays().Size(), 0);
        EXPECT_EQ(view_info.GetOverlays()[0].GetSprite(), "bom bom");
        EXPECT_EQ(view_info.GetOverlays()[0].GetState(), "bim bom");
        overlay.SetSprite("not bim bom");
        overlay.SetState("not bom bom");
        EXPECT_EQ(view_info.GetOverlays()[0].GetSprite(), "not bim bom");
        EXPECT_EQ(view_info.GetOverlays()[0].GetState(), "not bom bom");
    }

    {
        view_info.AddOverlay("bom bom2", "bim bom2");
        ASSERT_EQ(view_info.GetOverlays().Size(), 2);
        ASSERT_EQ(view_info.GetUnderlays().Size(), 0);
        EXPECT_EQ(view_info.GetOverlays()[1].GetSprite(), "bom bom2");
        EXPECT_EQ(view_info.GetOverlays()[1].GetState(), "bim bom2");
    }

    {
        ViewInfo::FramesetInfo underlay
            = view_info.AddUnderlay("image", "state");
        ASSERT_EQ(view_info.GetOverlays().Size(), 2);
        ASSERT_EQ(view_info.GetUnderlays().Size(), 1);
        EXPECT_EQ(view_info.GetUnderlays()[0].GetSprite(), "image");
        EXPECT_EQ(view_info.GetUnderlays()[0].GetState(), "state");
        underlay.SetSprite("image2");
        underlay.SetState("state2");
        EXPECT_EQ(view_info.GetUnderlays()[0].GetSprite(), "image2");
        EXPECT_EQ(view_info.GetUnderlays()[0].GetState(), "state2");
    }

    {
        view_info.AddUnderlay("image2", "state2");
        ASSERT_EQ(view_info.GetOverlays().Size(), 2);
        ASSERT_EQ(view_info.GetUnderlays().Size(), 2);
        EXPECT_EQ(view_info.GetUnderlays()[1].GetSprite(), "image2");
        EXPECT_EQ(view_info.GetUnderlays()[1].GetState(), "state2");
    }

    view_info.RemoveOverlays();
    EXPECT_EQ(view_info.GetOverlays().Size(), 0);
    EXPECT_EQ(view_info.GetUnderlays().Size(), 2);

    view_info.RemoveUnderlays();
    EXPECT_EQ(view_info.GetOverlays().Size(), 0);
    EXPECT_EQ(view_info.GetUnderlays().Size(), 0);
}

TEST(ViewInfo, IsSameFramesets)
{
    ViewInfo view_info;
    EXPECT_TRUE(ViewInfo::IsSameFramesets(view_info, view_info));

    view_info.SetAngle(10);
    view_info.SetTransparency(555);
    view_info.SetSprite("sprite");
    view_info.SetState("state");
    view_info.AddOverlay("1", "1");
    view_info.AddUnderlay("2", "2");
    EXPECT_TRUE(ViewInfo::IsSameFramesets(view_info, view_info));

    ViewInfo view_info2;
    EXPECT_FALSE(ViewInfo::IsSameFramesets(view_info, view_info2));

    view_info2.SetAngle(11);
    view_info2.SetSprite("sprite");
    view_info2.SetState("state");
    EXPECT_FALSE(ViewInfo::IsSameFramesets(view_info, view_info2));

    view_info2.SetAngle(10);
    view_info2.SetTransparency(555);
    view_info2.SetSprite("sprite1");
    view_info2.SetState("state");
    EXPECT_FALSE(ViewInfo::IsSameFramesets(view_info, view_info2));

    view_info2.SetAngle(10);
    view_info2.SetSprite("sprite");
    view_info2.SetState("state1");
    EXPECT_FALSE(ViewInfo::IsSameFramesets(view_info, view_info2));

    view_info2.AddUnderlay("vvv", "wwww");
    EXPECT_FALSE(ViewInfo::IsSameFramesets(view_info, view_info2));

    view_info2.RemoveUnderlays();
    view_info2.AddUnderlay("2", "2");
    EXPECT_FALSE(ViewInfo::IsSameFramesets(view_info, view_info2));

    view_info2.SetAngle(10);
    view_info2.SetSprite("sprite");
    view_info2.SetState("state");
    EXPECT_FALSE(ViewInfo::IsSameFramesets(view_info, view_info2));

    view_info2.AddOverlay("vvv", "wwww");
    EXPECT_FALSE(ViewInfo::IsSameFramesets(view_info, view_info2));

    view_info2.RemoveOverlays();
    view_info2.AddOverlay("1", "1");
    EXPECT_TRUE(ViewInfo::IsSameFramesets(view_info, view_info2));

    view_info2.SetTransparency(554);
    EXPECT_FALSE(ViewInfo::IsSameFramesets(view_info, view_info2));

    view_info2.SetTransparency(555);
    view_info2.AddOverlay("111", "111");
    EXPECT_FALSE(ViewInfo::IsSameFramesets(view_info, view_info2));

    view_info2.RemoveOverlays();
    view_info2.AddOverlay("1", "1");
    view_info2.AddUnderlay("2", "2");
    EXPECT_FALSE(ViewInfo::IsSameFramesets(view_info, view_info2));

    view_info2.RemoveUnderlays();
    view_info2.AddUnderlay("2", "2");
    view_info2.SetAngle(88);
    EXPECT_FALSE(ViewInfo::IsSameFramesets(view_info, view_info2));

    view_info2.SetAngle(10);
    view_info2.SetState("spqr");
    EXPECT_FALSE(ViewInfo::IsSameFramesets(view_info, view_info2));

    view_info2.SetState("state");
    view_info2.RemoveUnderlays();
    view_info2.AddUnderlay("2fkds;a", "2fdjsl;kjf");
    EXPECT_FALSE(ViewInfo::IsSameFramesets(view_info, view_info2));
}

TEST(ViewInfo, StreamOperators)
{
    ViewInfo view_info;

    view_info.SetSprite("vhs");
    view_info.SetState("dead");
    view_info.SetAngle(11);
    view_info.SetTransparency(8989);
    view_info.AddOverlay("something", "someone").SetShift(1, 10);
    view_info.AddUnderlay("tree", "weed");

    kv::FastSerializer serializer(1);
    serializer << view_info;

    kv::FastDeserializer deserializer(serializer.GetData(), serializer.GetIndex());

    ViewInfo view_info2;
    deserializer >> view_info2;
    EXPECT_EQ(view_info2.GetAngle(), 11);
    EXPECT_EQ(view_info2.GetTransparency(), 8989);
    EXPECT_EQ(view_info2.GetBaseFrameset().GetSprite(), "vhs");
    EXPECT_EQ(view_info2.GetBaseFrameset().GetState(), "dead");

    ASSERT_EQ(view_info2.GetOverlays().Size(), 1);
    ViewInfo::ConstFramesetInfo frameset_info1 = view_info2.GetOverlays()[0];
    EXPECT_EQ(frameset_info1.GetSprite(), "something");
    EXPECT_EQ(frameset_info1.GetState(), "someone");
    EXPECT_EQ(frameset_info1.GetShiftX(), 1);
    EXPECT_EQ(frameset_info1.GetShiftY(), 10);

    ASSERT_EQ(view_info2.GetUnderlays().Size(), 1);
    ViewInfo::ConstFramesetInfo frameset_info2 = view_info2.GetUnderlays()[0];
    EXPECT_EQ(frameset_info2.GetSprite(), "tree");
    EXPECT_EQ(frameset_info2.GetState(), "weed");

    EXPECT_TRUE(ViewInfo::IsSameFramesets(view_info, view_info2));
}

TEST(ViewInfo, Hash)
{
    ViewInfo view_info;
    EXPECT_EQ(Hash(view_info), 100002);

    view_info.SetAngle(10);
    EXPECT_EQ(Hash(view_info), 100012);

    view_info.SetTransparency(1);
    EXPECT_EQ(Hash(view_info), 13);

    view_info.SetSprite("sprite");
    EXPECT_EQ(Hash(view_info), 2514701248);

    view_info.SetState("state");
    EXPECT_EQ(Hash(view_info), 227251335);

    view_info.AddOverlay("1", "1");
    EXPECT_EQ(Hash(view_info), 241341979);

    view_info.AddUnderlay("2", "2");
    EXPECT_EQ(Hash(view_info), 834067625);
}
