#include "objects/mobs/HumanInterface.h"

#include <gtest/gtest.h>

#include "ObjectFactory.h"
#include "interfaces_mocks.h"

#include "objects/movable/items/Item.h"

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
    EXPECT_EQ(view_info.GetOverlays().Size(), 0);
    EXPECT_EQ(view_info.GetUnderlays().Size(), 0);

    const ViewInfo empty_view_info;
    EXPECT_EQ(view_info.GetRawData(), empty_view_info.GetRawData());
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

TEST_F(HumanInterfaceTest, RemoveItemByItem)
{
    // TODO (?): replace it with real human object
    interface->SetOwner(42);

    using namespace kv::slot;

    const QVector<std::pair<kv::SlotType, QString>> TYPE_AND_SLOT_LIST
        = { {kv::SlotType::DEFAULT, LEFT_HAND},
            {kv::SlotType::DEFAULT, RIGHT_HAND},
            {kv::SlotType::SUIT, SUIT},
            {kv::SlotType::UNIFORM, UNIFORM},
            {kv::SlotType::FEET, FEET},
            {kv::SlotType::HEAD, HEAD}
          };

    QVector<IdPtr<kv::Item>> items;

    for (const auto& type_and_slot : TYPE_AND_SLOT_LIST)
    {
        IdPtr<kv::Item> item = factory.CreateImpl(kv::Item::GetTypeStatic());
        item->type = type_and_slot.first;

        ASSERT_TRUE(interface->InsertItem(type_and_slot.second, item));
        ASSERT_EQ(interface->GetItem(type_and_slot.second), item);
        ASSERT_EQ(item->GetOwner().Id(), 42);
        items.append(item);
    }

    for (int item_id = 0; item_id < items.size(); ++item_id)
    {
        interface->RemoveItem(items[item_id]);

        for (int before_item_id = 0; before_item_id <= item_id; ++before_item_id)
        {
            EXPECT_FALSE(interface->GetItem(TYPE_AND_SLOT_LIST[before_item_id].second).IsValid());
            EXPECT_EQ(items[item_id]->GetOwner().Id(), 42);
        }

        for (int after_item_id = item_id + 1; after_item_id < items.size(); ++after_item_id)
        {
            EXPECT_EQ(interface->GetItem(TYPE_AND_SLOT_LIST[after_item_id].second), items[after_item_id]);
        }
    }
}
