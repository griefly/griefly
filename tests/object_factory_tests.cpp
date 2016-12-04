#include <gtest/gtest.h>

#include "interfaces_mocks.h"

#include "core/ObjectFactory.h"

#include "core/objects/MainObject.h"
#include "core/objects/test/UnsyncGenerator.h"
#include "core/objects/test/TestMainObject.h"

using ::testing::ReturnRef;

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
        EXPECT_EQ(object->GetId(), 1);
    }
    {
        quint32 id = factory.CreateImpl(UnsyncGenerator::T_ITEM_S());
        EXPECT_EQ(id, 2);

        ASSERT_GT(factory.GetIdTable().size(), 3);
        IMainObject* object = factory.GetIdTable()[2].object;
        EXPECT_EQ(object->T_ITEM(), UnsyncGenerator::T_ITEM_S());
        EXPECT_EQ(object->GetId(), 2);
    }
}

TEST(ObjectFactory, AfterWorldCreation)
{
    MockIGame game;
    ObjectFactory factory(&game);

    {
        factory.BeginWorldCreation();

        quint32 id = factory.CreateImpl(TestMainObject::T_ITEM_S());
        ASSERT_EQ(id, 1);

        ASSERT_GT(factory.GetIdTable().size(), 2);
        IMainObject* object = factory.GetIdTable()[1].object;
        ASSERT_EQ(object->T_ITEM(), TestMainObject::T_ITEM_S());

        TestMainObject* test_object = static_cast<TestMainObject*>(object);
        ASSERT_EQ(test_object->after_world_creation_, 0);

        factory.FinishWorldCreation();
        ASSERT_EQ(test_object->after_world_creation_, 1);
    }
    {
        quint32 id = factory.CreateImpl(TestMainObject::T_ITEM_S());
        ASSERT_EQ(id, 2);

        ASSERT_GT(factory.GetIdTable().size(), 3);
        IMainObject* object = factory.GetIdTable()[2].object;
        ASSERT_EQ(object->T_ITEM(), TestMainObject::T_ITEM_S());

        TestMainObject* test_object = static_cast<TestMainObject*>(object);
        ASSERT_EQ(test_object->after_world_creation_, 1);
    }
}

TEST(ObjectFactory, Process)
{
    MockIGame game;
    ObjectFactory factory(&game);
    factory.FinishWorldCreation();

    {
        quint32 id = factory.CreateImpl(TestMainObject::T_ITEM_S());
        ASSERT_EQ(id, 1);
        ASSERT_GT(factory.GetIdTable().size(), 2);
        IMainObject* object = factory.GetIdTable()[1].object;
        ASSERT_EQ(object->T_ITEM(), TestMainObject::T_ITEM_S());

        TestMainObject* test_object = static_cast<TestMainObject*>(object);
        EXPECT_EQ(test_object->process_, 0);

        MAIN_TICK = 1;

        factory.ForeachProcess();
        EXPECT_EQ(test_object->process_, 0);

        EXPECT_CALL(game, GetFactory())
            .WillOnce(ReturnRef(factory));
        test_object->SetFreq(1);
        factory.ForeachProcess();
        EXPECT_EQ(test_object->process_, 1);

        factory.ForeachProcess();
        EXPECT_EQ(test_object->process_, 2);

        EXPECT_CALL(game, GetFactory())
            .WillOnce(ReturnRef(factory));
        test_object->SetFreq(10);
        factory.ForeachProcess();
        EXPECT_EQ(test_object->process_, 2);

        MAIN_TICK = 100;
        factory.ForeachProcess();
        EXPECT_EQ(test_object->process_, 3);

        factory.ForeachProcess();
        EXPECT_EQ(test_object->process_, 4);

        test_object->SetFreq(0);
        factory.ForeachProcess();
        EXPECT_EQ(test_object->process_, 4);

        factory.ForeachProcess();
        EXPECT_EQ(test_object->process_, 4);

        EXPECT_CALL(game, GetFactory())
            .WillOnce(ReturnRef(factory));
        test_object->SetFreq(1);
        factory.GetIdTable()[1].object = nullptr;
        factory.ForeachProcess();
        EXPECT_EQ(test_object->process_, 4);
        factory.GetIdTable()[1].object = object;

        quint32 id2 = factory.CreateImpl(TestMainObject::T_ITEM_S());
        ASSERT_EQ(id2, 2);
        ASSERT_GT(factory.GetIdTable().size(), 3);
        IMainObject* object2 = factory.GetIdTable()[2].object;
        ASSERT_EQ(object2->T_ITEM(), TestMainObject::T_ITEM_S());

        TestMainObject* test_object2 = static_cast<TestMainObject*>(object2);
        EXPECT_EQ(test_object2->process_, 0);

        test_object->SetProcessCallback(
        [&object2]()
        {
            object2->SetFreq(0);
        });
        EXPECT_CALL(game, GetFactory())
            .WillOnce(ReturnRef(factory));
        object2->SetFreq(1);

        factory.ForeachProcess();
        EXPECT_EQ(test_object2->process_, 0);

        test_object->SetProcessCallback(
        [&factory]()
        {
            factory.GetIdTable()[2].object = nullptr;
        });
        EXPECT_CALL(game, GetFactory())
            .WillOnce(ReturnRef(factory));
        object2->SetFreq(1);
        factory.ForeachProcess();
        EXPECT_EQ(test_object2->process_, 0);
        factory.GetIdTable()[2].object = object2;

        object->SetFreq(0);
        EXPECT_CALL(game, GetFactory())
            .WillRepeatedly(ReturnRef(factory));
        object2->SetFreq(1);
        object2->SetFreq(0);
        object2->SetFreq(1);
        object2->SetFreq(1);
        object2->SetFreq(20);
        object2->SetFreq(0);
        factory.ForeachProcess();
        EXPECT_EQ(test_object2->process_, 0);
    }
}

TEST(ObjectFactory, DeleteLater)
{
    MockIGame game;
    ObjectFactory factory(&game);
    factory.FinishWorldCreation();

    {
        quint32 id = factory.CreateImpl(TestMainObject::T_ITEM_S());
        ASSERT_EQ(id, 1);
        ASSERT_GT(factory.GetIdTable().size(), 2);
        IMainObject* object = factory.GetIdTable()[1].object;
        ASSERT_EQ(object->T_ITEM(), TestMainObject::T_ITEM_S());

        TestMainObject* test_object = static_cast<TestMainObject*>(object);
        int counter = 0;
        test_object->SetDestructorCallback(
        [&counter]()
        {
            ++counter;
        });
        EXPECT_EQ(counter, 0);
        EXPECT_TRUE(factory.GetIdTable()[1].object);
        factory.DeleteLater(1);
        EXPECT_EQ(counter, 0);
        EXPECT_FALSE(factory.GetIdTable()[1].object);
        factory.DeleteLater(1);
        EXPECT_EQ(counter, 0);
        EXPECT_FALSE(factory.GetIdTable()[1].object);
        factory.DeleteLater(1);
        EXPECT_EQ(counter, 0);
        EXPECT_FALSE(factory.GetIdTable()[1].object);
        factory.DeleteLater(1);
        EXPECT_EQ(counter, 0);
        EXPECT_FALSE(factory.GetIdTable()[1].object);
        factory.ProcessDeletion();
        EXPECT_EQ(counter, 1);
        EXPECT_FALSE(factory.GetIdTable()[1].object);
    }
}

TEST(ObjectFactory, PlayersIds)
{
    MockIGame game;
    ObjectFactory factory(&game);

    for (int i = 0; i < 100; ++i)
    {
        EXPECT_EQ(factory.GetNetId(i), 0);
        EXPECT_EQ(factory.GetPlayerId(i), 0);
    }

    factory.SetPlayerId(1, 9915);
    EXPECT_EQ(factory.GetNetId(9915), 1);
    EXPECT_EQ(factory.GetPlayerId(1), 9915);

    for (int i = 100; i < 200; ++i)
    {
        EXPECT_EQ(factory.GetNetId(i), 0);
        EXPECT_EQ(factory.GetPlayerId(i), 0);
    }

    factory.SetPlayerId(543, 10000000);
    EXPECT_EQ(factory.GetNetId(10000000), 543);
    EXPECT_EQ(factory.GetPlayerId(543), 10000000);
}

