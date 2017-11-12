#include <gtest/gtest.h>

#include "core/objects/mobs/Slot.h"

using namespace kv;

TEST(SlotType, SaveableOperators)
{
    const QVector<QPair<SlotType, quint32>> data
        = { {SlotType::ANYTHING, 3},
            {SlotType::DEFAULT, 0},
            {SlotType::FEET, 4},
            {SlotType::HEAD, 2},
            {SlotType::SUIT, 1},
            {SlotType::UNIFORM, 5} };

    for (const auto slot_and_hash : data)
    {
        FastSerializer serializer(1);
        serializer << slot_and_hash.first;
        FastDeserializer deserializer(serializer.GetData(), serializer.GetIndex());
        SlotType slot_type;
        deserializer >> slot_type;

        EXPECT_EQ(slot_type, slot_and_hash.first);
        EXPECT_EQ(Hash(slot_type), slot_and_hash.second);
    }
}

TEST(Button, SaveableOperators)
{
    Button button;
    button.name = "rainbow";
    button.position = {9, 4};
    button.view.SetSprite("reddish");

    FastSerializer serializer(1);
    serializer << button;

    FastDeserializer deserializer(serializer.GetData(), serializer.GetIndex());
    Button loaded_button;
    deserializer >> loaded_button;

    EXPECT_EQ(button.name, loaded_button.name);
    EXPECT_EQ(button.position, loaded_button.position);

    // TODO: operator==?
    // EXPECT_TRUE(ViewInfo::IsSameFramesets(button.view, loaded_button.view));

    EXPECT_EQ(Hash(loaded_button), 1546348643);
}

TEST(Slot, SaveableOperators)
{
    Slot slot;
    slot.item = 1;
    slot.name = "leg";
    slot.position = {5, 6};
    slot.type = SlotType::FEET;
    slot.view.SetSprite("test");
    slot.view.SetState("test2");
    slot.overlay_sprite = "bear";
    slot.overlay_state_postfix = "_aux";

    FastSerializer serializer(1);
    serializer << slot;

    FastDeserializer deserializer(serializer.GetData(), serializer.GetIndex());
    Slot loaded_slot;
    deserializer >> loaded_slot;

    EXPECT_EQ(slot.item, loaded_slot.item);
    EXPECT_EQ(slot.name, loaded_slot.name);
    EXPECT_EQ(slot.position, loaded_slot.position);
    EXPECT_EQ(slot.type, loaded_slot.type);
    EXPECT_EQ(slot.overlay_sprite, loaded_slot.overlay_sprite);
    EXPECT_EQ(slot.overlay_state_postfix, loaded_slot.overlay_state_postfix);

    // TODO: operator== ?
    //EXPECT_TRUE(ViewInfo::IsSameFramesets(slot.view, loaded_slot.view));

    EXPECT_EQ(Hash(loaded_slot), 1163438966);
}
