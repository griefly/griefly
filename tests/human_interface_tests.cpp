#include "core/objects/mobs/HumanInterface.h"

#include <gtest/gtest.h>

#include "core/ObjectFactory.h"
#include "interfaces_mocks.h"

#include "core/objects/movable/items/Item.h"

namespace
{

const QString VALID_SLOT_NAMES[]
    = { kv::slot::LEFT_HAND,
        kv::slot::RIGHT_HAND,
        kv::slot::HEAD,
        kv::slot::SUIT,
        kv::slot::UNIFORM,
        kv::slot::FEET };

const QString INVALID_SLOT_NAMES[]
    = { "BAD SLOT",
        "WORSE SLOT" };

}

class HumanInterfaceTest : public ::testing::Test
{
protected:
    MockIGame game;
    ObjectFactory factory;
    IdPtr<kv::HumanInterface> interface;
    HumanInterfaceTest()
        : factory(&game)
    {
        interface = factory.CreateImpl(kv::HumanInterface::GetTypeStatic());
    }
};

TEST_F(HumanInterfaceTest, Constructor)
{
    for (const QString& slot : VALID_SLOT_NAMES)
    {
        const IdPtr<kv::Item> item = interface->GetItem(slot);
        EXPECT_FALSE(item.IsValid());
    }
    for (const QString& slot : INVALID_SLOT_NAMES)
    {
        const IdPtr<kv::Item> item = interface->GetItem(slot);
        EXPECT_FALSE(item.IsValid());
    }

    EXPECT_FALSE(interface->GetItemInActiveHand().IsValid());

    ViewInfo view_info;
    interface->AddOverlays(&view_info);
    EXPECT_EQ(view_info.GetOverlays().size(), 0);
    EXPECT_EQ(view_info.GetUnderlays().size(), 0);

    const ViewInfo empty_view_info;
    EXPECT_TRUE(ViewInfo::IsSameFramesets(view_info, empty_view_info));
}

TEST_F(HumanInterfaceTest, PickDropItem)
{
    // TODO (?): replace it with real human object
    interface->SetOwner(42);

    const QVector<kv::SlotType> SLOT_TYPES
        = { kv::SlotType::DEFAULT,
            kv::SlotType::SUIT,
            kv::SlotType::HEAD,
            kv::SlotType::ANYTHING,
            kv::SlotType::FEET,
            kv::SlotType::UNIFORM };

    for (int slot_index = 0; slot_index <= SLOT_TYPES.size(); ++slot_index)
    {
        IdPtr<kv::Item> item = factory.CreateImpl(kv::Item::GetTypeStatic());
        EXPECT_FALSE(item->GetOwner().IsValid());

        if (slot_index != SLOT_TYPES.size())
        {
            item->type = SLOT_TYPES[slot_index];
        }

        EXPECT_TRUE(interface->PickItem(item));
        EXPECT_EQ(item->GetOwner().Id(), 42);

        EXPECT_EQ(interface->GetItemInActiveHand().Id(), item.Id());
        interface->DropItem();
        EXPECT_EQ(interface->GetItemInActiveHand().Id(), 0);
        // DropItem should not do anything with item itself
        EXPECT_EQ(item->GetOwner().Id(), 42);
    }
}

TEST_F(HumanInterfaceTest, InsertRemoveGetItem)
{
    // TODO (?): replace it with real human object
    interface->SetOwner(42);

    using namespace kv::slot;

    // <Slot type, appropriate slots, inapropriate slots>
    using SlotTypeAndSlots = std::tuple<kv::SlotType, QVector<QString>, QVector<QString>>;
    const QVector<SlotTypeAndSlots> SLOT_TYPES_AND_SLOTS
        { SlotTypeAndSlots{kv::SlotType::DEFAULT, {LEFT_HAND, RIGHT_HAND}, {HEAD, SUIT, UNIFORM, FEET}},
          SlotTypeAndSlots{kv::SlotType::HEAD, {LEFT_HAND, RIGHT_HAND, HEAD}, {SUIT, UNIFORM, FEET}},
          SlotTypeAndSlots{kv::SlotType::UNIFORM, {LEFT_HAND, RIGHT_HAND, UNIFORM}, {HEAD, SUIT, FEET}},
          SlotTypeAndSlots{kv::SlotType::FEET, {LEFT_HAND, RIGHT_HAND, FEET}, {HEAD, SUIT, UNIFORM}},
          SlotTypeAndSlots{kv::SlotType::SUIT, {LEFT_HAND, RIGHT_HAND, SUIT}, {HEAD, UNIFORM, FEET}}
        };

    for (const auto& tuple : SLOT_TYPES_AND_SLOTS)
    {
        const kv::SlotType slot_type = std::get<0>(tuple);
        const QVector<QString> appropriate_slots = std::get<1>(tuple);
        const QVector<QString> inappropriate_slots = std::get<2>(tuple);

        for (const QString& slot : appropriate_slots)
        {
            IdPtr<kv::Item> item = factory.CreateImpl(kv::Item::GetTypeStatic());
            item->type = slot_type;

            IdPtr<kv::Item> second_item = factory.CreateImpl(kv::Item::GetTypeStatic());
            second_item->type = slot_type;

            EXPECT_FALSE(interface->GetItem(slot).IsValid());
            EXPECT_FALSE(interface->InsertItem(slot, 0));
            EXPECT_TRUE(interface->InsertItem(slot, item));
            EXPECT_EQ(item->GetOwner().Id(), 42);
            EXPECT_EQ(interface->GetItem(slot), item);
            EXPECT_FALSE(interface->InsertItem(slot, second_item));
            EXPECT_FALSE(second_item->GetOwner().IsValid());
            EXPECT_EQ(item->GetOwner().Id(), 42);
            EXPECT_EQ(interface->GetItem(slot), item);
            interface->RemoveItem(slot);
            EXPECT_FALSE(interface->GetItem(slot).IsValid());
            EXPECT_EQ(item->GetOwner().Id(), 42);
        }

        for (const QString& slot : inappropriate_slots)
        {
            IdPtr<kv::Item> item = factory.CreateImpl(kv::Item::GetTypeStatic());
            item->type = slot_type;

            EXPECT_FALSE(interface->GetItem(slot).IsValid());
            EXPECT_FALSE(interface->InsertItem(slot, 0));
            EXPECT_FALSE(interface->InsertItem(slot, item));
            EXPECT_FALSE(item->GetOwner().IsValid());
            EXPECT_FALSE(interface->GetItem(slot).IsValid());
            interface->RemoveItem(slot);
            EXPECT_FALSE(interface->GetItem(slot).IsValid());
            EXPECT_FALSE(item->GetOwner().IsValid());
        }
    }
}
