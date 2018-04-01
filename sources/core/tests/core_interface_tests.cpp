#include "core_headers/CoreInterface.h"

#include <gtest/gtest.h>

#include "objects/test/PressureIndicator.h"
#include "objects/turfs/SpaceTurf.h"

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

namespace
{

void EntityExpectEq(const kv::FrameData::Entity& left, const kv::FrameData::Entity& right)
{
    EXPECT_EQ(left.view, right.view);
    EXPECT_EQ(left.click_id, right.click_id);
    EXPECT_EQ(left.dir, right.dir);
    EXPECT_EQ(left.id, right.id);
    EXPECT_EQ(left.pos_x, right.pos_x);
    EXPECT_EQ(left.pos_y, right.pos_y);
    EXPECT_EQ(left.vlevel, right.vlevel);
}

void UnitExpectEq(const kv::FrameData::InterfaceUnit& left, const kv::FrameData::InterfaceUnit& right)
{
    EXPECT_EQ(left.view, right.view);
    EXPECT_EQ(left.pixel_x, right.pixel_x);
    EXPECT_EQ(left.pixel_y, right.pixel_y);
    EXPECT_EQ(left.name, right.name);
    EXPECT_EQ(left.shift, right.shift);
}

void SoundExpectEq(const kv::FrameData::Sound& left, const kv::FrameData::Sound& right)
{
    EXPECT_EQ(left.name, right.name);
}

void ChatExpectEq(const kv::FrameData::ChatMessage& left, const kv::FrameData::ChatMessage& right)
{
    EXPECT_EQ(left.html, right.html);
}

void TextExpectEq(const kv::FrameData::TextEntry& left, const kv::FrameData::TextEntry& right)
{
    EXPECT_EQ(left.tab, right.tab);
    EXPECT_EQ(left.text, right.text);
}

}

TEST(CoreInterface, GrowingFrame)
{
    kv::FrameData frame;
    kv::GrowingFrame growing(&frame);

    growing.SetCamera(43, 100);
    EXPECT_EQ(frame.camera_pos_x, 43);
    EXPECT_EQ(frame.camera_pos_y, 100);

    kv::FrameData::Music music;
    music.name = "test";
    music.volume = 14;
    growing.SetMusic(music);
    EXPECT_EQ(frame.music.name, "test");
    EXPECT_EQ(frame.music.volume, 14);

    kv::FrameData::Entity entity;
    entity.view.base_frameset.sprite_name = "sprite1";
    entity.view.base_frameset.state = "state2";
    entity.click_id = 32;
    entity.dir = Dir::EAST;
    entity.id = 888889;
    entity.pos_x = 50;
    entity.pos_y = 60;
    entity.vlevel = 10;

    growing.Append(entity);

    ASSERT_EQ(frame.entities.size(), 1);
    EntityExpectEq(entity, frame.entities[0]);
    ASSERT_EQ(frame.units.size(), 0);
    ASSERT_EQ(frame.sounds.size(), 0);
    ASSERT_EQ(frame.messages.size(), 0);
    ASSERT_EQ(frame.texts.size(), 0);

    kv::FrameData::InterfaceUnit unit;
    unit.view.base_frameset.sprite_name = "unit1";
    unit.view.base_frameset.state = "unit2";
    unit.pixel_x = 444;
    unit.pixel_y = 1111;
    unit.name = "button";
    unit.shift = 1;

    growing.Append(unit);

    ASSERT_EQ(frame.entities.size(), 1);
    EntityExpectEq(entity, frame.entities[0]);
    ASSERT_EQ(frame.units.size(), 1);
    UnitExpectEq(unit, frame.units[0]);
    ASSERT_EQ(frame.sounds.size(), 0);
    ASSERT_EQ(frame.messages.size(), 0);
    ASSERT_EQ(frame.texts.size(), 0);

    kv::FrameData::Sound sound;
    sound.name = "sound4";

    growing.Append(sound);

    ASSERT_EQ(frame.entities.size(), 1);
    EntityExpectEq(entity, frame.entities[0]);
    ASSERT_EQ(frame.units.size(), 1);
    UnitExpectEq(unit, frame.units[0]);
    ASSERT_EQ(frame.sounds.size(), 1);
    SoundExpectEq(sound, frame.sounds[0]);
    ASSERT_EQ(frame.messages.size(), 0);
    ASSERT_EQ(frame.texts.size(), 0);

    kv::FrameData::ChatMessage message;
    message.html = "<b>WOW</b>";

    growing.Append(message);

    ASSERT_EQ(frame.entities.size(), 1);
    EntityExpectEq(entity, frame.entities[0]);
    ASSERT_EQ(frame.units.size(), 1);
    UnitExpectEq(unit, frame.units[0]);
    ASSERT_EQ(frame.sounds.size(), 1);
    SoundExpectEq(sound, frame.sounds[0]);
    ASSERT_EQ(frame.messages.size(), 1);
    ChatExpectEq(message, frame.messages[0]);
    ASSERT_EQ(frame.texts.size(), 0);

    kv::FrameData::TextEntry text;
    text.tab = "tab9";
    text.text = "Info";

    growing.Append(text);

    ASSERT_EQ(frame.entities.size(), 1);
    EntityExpectEq(entity, frame.entities[0]);
    ASSERT_EQ(frame.units.size(), 1);
    UnitExpectEq(unit, frame.units[0]);
    ASSERT_EQ(frame.sounds.size(), 1);
    SoundExpectEq(sound, frame.sounds[0]);
    ASSERT_EQ(frame.messages.size(), 1);
    ChatExpectEq(message, frame.messages[0]);
    ASSERT_EQ(frame.texts.size(), 1);
    TextExpectEq(text, frame.texts[0]);
}

TEST(CoreInterface, ObjectsMetadata)
{
    const kv::CoreInterface::ObjectsMetadata metadata
        = kv::GetCoreInstance().GetObjectsMetadata();

    EXPECT_FALSE(metadata.isEmpty());

    for (auto it = metadata.begin(); it != metadata.end(); ++it)
    {
        EXPECT_EQ(it.key(), it.value().name);
    }

    // TODO (?): create special object
    ASSERT_TRUE(metadata.contains(kv::PressureIndicator::GetTypeStatic()));

    {
        const auto& indicator_metadata = metadata[kv::PressureIndicator::GetTypeStatic()];

        kv::RawViewInfo view = indicator_metadata.default_view;
        EXPECT_EQ(view.base_frameset.sprite_name, "icons/numbers.dmi");
        EXPECT_EQ(view.base_frameset.state, "empty");

        EXPECT_FALSE(indicator_metadata.turf);
        ASSERT_EQ(indicator_metadata.variables.size(), 11);

        EXPECT_TRUE(indicator_metadata.variables.contains("transparent"));
        EXPECT_TRUE(indicator_metadata.variables.contains("name_"));
        EXPECT_TRUE(indicator_metadata.variables.contains("view_"));
        EXPECT_TRUE(indicator_metadata.variables.contains("owner_"));
        EXPECT_FALSE(indicator_metadata.variables.contains("id_"));
    }

    {
        const auto& space_metadata = metadata[kv::Space::GetTypeStatic()];
        kv::RawViewInfo view = space_metadata.default_view;
        EXPECT_EQ(view.base_frameset.sprite_name, "icons/space.dmi");

        EXPECT_TRUE(space_metadata.turf);
        ASSERT_EQ(space_metadata.variables.size(), 13);

        EXPECT_TRUE(space_metadata.variables.contains("transparent"));
        EXPECT_TRUE(space_metadata.variables.contains("name_"));
        EXPECT_TRUE(space_metadata.variables.contains("view_"));
        EXPECT_TRUE(space_metadata.variables.contains("owner_"));
        EXPECT_TRUE(space_metadata.variables.contains("atmos_state_"));
        EXPECT_FALSE(space_metadata.variables.contains("id_"));
    }
}
