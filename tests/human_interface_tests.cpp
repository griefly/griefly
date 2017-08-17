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
    IdPtr<kv::HumanInterface2> interface;
    HumanInterfaceTest()
        : factory(&game)
    {
        interface = factory.CreateImpl(kv::HumanInterface2::GetTypeStatic());
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
    }
}
