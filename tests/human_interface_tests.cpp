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

TEST(HumanInterface, Constructor)
{
    MockIGame game;
    ObjectFactory factory(&game);

    IdPtr<kv::HumanInterface2> interface
        = factory.CreateImpl(kv::HumanInterface2::GetTypeStatic());

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


    ViewInfo view_info;
    interface->AddOverlays(&view_info);
    EXPECT_EQ(view_info.GetOverlays().size(), 0);
    EXPECT_EQ(view_info.GetUnderlays().size(), 0);

    const ViewInfo empty_view_info;
    EXPECT_TRUE(ViewInfo::IsSameFramesets(view_info, empty_view_info));
}
