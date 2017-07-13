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
