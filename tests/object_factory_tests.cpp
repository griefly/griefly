#include <gtest/gtest.h>

#include "interfaces_mocks.h"

#include "core/ObjectFactory.h"

#include "core/objects/MainObject.h"
#include "core/objects/test/UnsyncGenerator.h"

TEST(ObjectFactory, Constructor)
{
    MockIGame game;
    ObjectFactory factory(&game);
    EXPECT_EQ(factory.Hash(), 0);
    for (auto info : factory.GetIdTable())
    {
        EXPECT_EQ(info.object, nullptr);
    }
    EXPECT_EQ(id_ptr_id_table, &factory.GetIdTable());
}

TEST(ObjectFactory, CreateImpl)
{
    MockIGame game;
    ObjectFactory factory(&game);
    {
        quint32 id = factory.CreateImpl(IMainObject::T_ITEM_S());
        EXPECT_EQ(id, 1);

        ASSERT_GT(factory.GetIdTable().size(), 2);
        IMainObject* object = factory.GetIdTable()[1].object;
        EXPECT_EQ(object->T_ITEM(), IMainObject::T_ITEM_S());
    }
    {
        quint32 id = factory.CreateImpl(UnsyncGenerator::T_ITEM_S());
        EXPECT_EQ(id, 2);

        ASSERT_GT(factory.GetIdTable().size(), 3);
        IMainObject* object = factory.GetIdTable()[2].object;
        EXPECT_EQ(object->T_ITEM(), UnsyncGenerator::T_ITEM_S());
    }
}
