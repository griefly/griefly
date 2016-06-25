#include <gtest/gtest.h>

#include "interfaces_mocks.h"

#include "core/objects/MainObject.h"

using ::testing::ReturnRef;
using ::testing::Return;

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
        object.Save(save);
        ASSERT_EQ(save.str(), " main  42  0 ");
    }
    {
        MockIGame game;
        MockIObjectFactory factory;
        EXPECT_CALL(game, GetFactory())
            .WillRepeatedly(ReturnRef(factory));
        EXPECT_CALL(factory, AddProcessingItem(42))
            .WillOnce(Return());

        IMainObject object(42);
        object.SetGame(&game);
        std::stringstream save("18");
        object.Load(save);
        ASSERT_EQ(object.GetFreq(), 18);
    }
}

TEST(MainObjectDeathTest, Deaths)
{
    {
        IMainObject object(42);
        ASSERT_DEATH(
        {
            object.GetGame();
        }, "IMainObject::GetGame\\(\\) is called during construction of object");
    }
    {
        IMainObject object(42);
        const IMainObject* ptr = &object;
        ASSERT_DEATH(
        {
            ptr->GetGame();
        }, "IMainObject::GetGame\\(\\) is called during construction of object");
    }
    {
        IMainObject object(42);
        ASSERT_DEATH(
        {
            object.SetFreq(0);
        }, "SetFreq is called in constructor");
    }
    {
        IMainObject object(0);
        MockIGame game;
        object.SetGame(&game);
        ASSERT_DEATH(
        {
            object.SetFreq(0);
        }, "GetId\\(\\) is zero");
    }
}

TEST(MainObject, SettersAndGettersAndCreateImpl)
{
    {
        MockIGame game;
        MockIObjectFactory factory;

        IMainObject object(43);
        object.SetGame(&game);
        IGame* interface_game = &object.GetGame();
        ASSERT_EQ(interface_game, &game);

        const IMainObject& object_const_ref = object;
        ASSERT_EQ(&object_const_ref.GetGame(), &game);

        EXPECT_CALL(game, GetFactory())
            .WillRepeatedly(ReturnRef(factory));
        ASSERT_EQ(&object.GetFactory(), &factory);

        SyncRandom random;
        random.SetRand(42, 42);
        EXPECT_CALL(game, GetRandom())
            .WillRepeatedly(ReturnRef(random));
        ASSERT_EQ(object.GetRand(), 5885939);
    }
    {
        MockIGame game;
        MockIObjectFactory factory;
        EXPECT_CALL(game, GetFactory())
            .WillRepeatedly(ReturnRef(factory));

        IMainObject object(43);
        object.SetGame(&game);
        ASSERT_EQ(object.GetFreq(), 0);

        EXPECT_CALL(factory, AddProcessingItem(43));
        object.SetFreq(46);
        ASSERT_EQ(object.GetFreq(), 46);

        EXPECT_CALL(factory, AddProcessingItem(84));
        object.SetId(84);
        ASSERT_EQ(object.GetId(), 84);
    }
    {
        MockIGame game;
        MockIObjectFactory factory;
        EXPECT_CALL(game, GetFactory())
            .WillRepeatedly(ReturnRef(factory));
        EXPECT_CALL(factory, CreateImpl("type", 42))
            .WillOnce(Return(111));

        IMainObject object(43);
        object.SetGame(&game);
        ASSERT_EQ(object.CreateImpl("type", 42), 111);
    }
}












