#include <gtest/gtest.h>

#include "core/objects/MainObject.h"

TEST(MainObject, Constructor)
{
    IMainObject object(42);
    ASSERT_EQ(object.GetId(), 42);
    ASSERT_EQ(object.GetFreq(), 0);

    IMainObject object2(/*NotLoadItem*/nouse);
    ASSERT_EQ(object2.GetId(), 0);
    ASSERT_EQ(object2.GetFreq(), 0);
}

TEST(MainObject, Types)
{
   ASSERT_EQ(IMainObject::T_ITEM_S(), "main");
   IMainObject object(42);
   ASSERT_EQ(object.T_ITEM(), "main");
}

TEST(MainObject, Save)
{
    {
        IMainObject object(42);
        std::stringstream save;
        object.SaveSelf(save);
        ASSERT_EQ(save.str(), " main  42  0 ");
    }
}
