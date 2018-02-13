#include <gtest/gtest.h>

#include "interfaces_mocks.h"

#include "objects/Object.h"

using ::testing::ReturnRef;
using ::testing::Return;

using namespace kv;

TEST(MainObject, Constructor)
{
    Object object;
    ASSERT_EQ(object.GetId(), 0);
    ASSERT_EQ(object.GetFreq(), 0);

    Object object2(kv::internal::no_initialization);
    ASSERT_EQ(object2.GetId(), 0);
    ASSERT_EQ(object2.GetFreq(), 0);
}

TEST(MainObject, Types)
{
   ASSERT_EQ(Object::GetTypeStatic(), "main");
   Object object;
   ASSERT_EQ(object.GetType(), "main");

   ASSERT_EQ(object.GetTypeIndex(), 0);
   ASSERT_EQ(Object::GetTypeIndexStatic(), 0);
}

TEST(MainObject, Hash)
{
    Object object;
    ASSERT_EQ(object.HashMembers(), 0);
}

TEST(MainObject, EmptyFunctions)
{
    // They just should not crash the game
    Object object;
    object.AfterWorldCreation();
    object.Process();
}

TEST(MainObject, Save)
{
    {
        Object object;
        kv::internal::GetObjectId(&object) = 42;
        FastSerializer save(1);
        object.Save(save);

        const char* const DATA =
            "\x06\x02\x04\x00\x00\x00\x6d\x00\x61\x00"
            "\x69\x00\x6E\x00\x03\x2A\x00\x00\x00"
            "\x02\x00\x00\x00\x00";

        EXPECT_EQ(
            QByteArray(save.GetData(), save.GetIndex()),
            QByteArray(DATA, 24));
    }
    {
        MockIGame game;
        MockObjectProcessor processor;
        EXPECT_CALL(game, GetProcessor())
            .WillRepeatedly(ReturnRef(processor));
        EXPECT_CALL(processor, Add(42))
            .WillOnce(Return());

        Object object;
        kv::internal::GetObjectId(&object) = 42;
        kv::internal::GetObjectGame(&object) = &game;
        FastDeserializer save("\x02\x08\x00\x00\x00", 5);
        object.Load(save);
        EXPECT_EQ(object.GetFreq(), 8);
    }
}

TEST(MainObjectDeathTest, Deaths)
{
    {
        Object object;
        ASSERT_DEATH(
        {
            object.GetGame();
        }, "Object::GetGame\\(\\) is called during construction of object");
    }
    {
        Object object;
        const Object* ptr = &object;
        ASSERT_DEATH(
        {
            ptr->GetGame();
        }, "Object::GetGame\\(\\) is called during construction of object");
    }
    {
        Object object;
        ASSERT_DEATH(
        {
            object.SetFreq(0);
        }, "SetFreq is called in constructor");
    }
    {
        Object object;
        MockIGame game;
        kv::internal::GetObjectGame(&object) = &game;
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

        Object object;
        kv::internal::GetObjectId(&object) = 43;
        kv::internal::GetObjectGame(&object) = &game;
        GameInterface* interface_game = &object.GetGame();
        ASSERT_EQ(interface_game, &game);

        const Object& object_const_ref = object;
        ASSERT_EQ(&object_const_ref.GetGame(), &game);

        EXPECT_CALL(game, GetFactory())
            .WillRepeatedly(ReturnRef(factory));
        ASSERT_EQ(&object.GetFactory(), &factory);
    }
    {
        MockIGame game;
        MockObjectProcessor processor;
        EXPECT_CALL(game, GetProcessor())
            .WillRepeatedly(ReturnRef(processor));

        Object object;
        kv::internal::GetObjectId(&object) = 43;
        kv::internal::GetObjectGame(&object) = &game;
        ASSERT_EQ(object.GetFreq(), 0);

        EXPECT_CALL(processor, Add(43));
        object.SetFreq(46);
        ASSERT_EQ(object.GetFreq(), 46);
    }
    {
        MockIGame game;
        MockIObjectFactory factory;
        EXPECT_CALL(game, GetFactory())
            .WillRepeatedly(ReturnRef(factory));
        EXPECT_CALL(factory, CreateImpl(QString("type"), 42))
            .WillOnce(Return(111));

        Object object;
        kv::internal::GetObjectId(&object) = 43;
        kv::internal::GetObjectGame(&object) = &game;
        ASSERT_EQ(kv::internal::CreateImpl(&object, "type", 42), 111);
    }
}

TEST(MainObject, Delete)
{
    MockIGame game;
    MockIObjectFactory factory;
    EXPECT_CALL(game, GetFactory())
        .WillRepeatedly(ReturnRef(factory));

    Object object;
    kv::internal::GetObjectId(&object) = 43;
    kv::internal::GetObjectGame(&object) = &game;

    EXPECT_CALL(factory, DeleteLater(43))
        .WillOnce(Return());
    object.Delete();
}
