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
        EXPECT_EQ(hash(slot_type), slot_and_hash.second);
    }
}

TEST(Slot, SaveableOperators)
{
    Slot slot;
    slot.item = 1;
    slot.name = "leg";
    slot.posx = 5;
    slot.posy = 6;
    slot.type = SlotType::FEET;
    slot.view.SetSprite("test");

    FastSerializer serializer(1);
    serializer << slot;

    FastDeserializer deserializer(serializer.GetData(), serializer.GetIndex());
    Slot loaded_slot;
    deserializer >> loaded_slot;

    EXPECT_EQ(slot.item, loaded_slot.item);
    EXPECT_EQ(slot.name, loaded_slot.name);
    EXPECT_EQ(slot.posx, loaded_slot.posx);
    EXPECT_EQ(slot.posy, loaded_slot.posy);
    EXPECT_EQ(slot.type, loaded_slot.type);

    EXPECT_TRUE(ViewInfo::IsSameFramesets(slot.view, loaded_slot.view));

    EXPECT_EQ(hash(loaded_slot), 2658126356);
}
