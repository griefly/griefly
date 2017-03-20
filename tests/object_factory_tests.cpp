#include <gtest/gtest.h>

#include "interfaces_mocks.h"

#include "core/ObjectFactory.h"

#include "core/objects/MainObject.h"
#include "core/objects/Tile.h"
#include "core/objects/OnMapObject.h"
#include "core/objects/Turf.h"
#include "core/objects/Mob.h"
#include "core/objects/test/TestMainObject.h"
#include "core/objects/test/UnsyncGenerator.h"

using ::testing::ReturnRef;
using ::testing::Return;

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

TEST(ObjectFactoryDeathTest, CreateImplFailAdd)
{
    MockIGame game;
    ObjectFactory factory(&game);

    ASSERT_DEATH(
    {
        quint32 id = factory.CreateImpl(IOnMapBase::GetTypeStatic());
        factory.CreateImpl(IOnMapBase::GetTypeStatic(), id);
    }, "AddItem failed");
}

TEST(ObjectFactoryDeathTest, CreateImplFailFindType)
{
    MockIGame game;
    ObjectFactory factory(&game);

    ASSERT_DEATH(
    {
        factory.CreateImpl("This type does not exist.");
    }, "Unable to find creator for type");
}

TEST(ObjectFactory, CreateImpl)
{
    MockIGame game;
    MockIMapMaster map;
    MockIAtmosphere atmos;
    ObjectFactory factory(&game);
    {
        quint32 id = factory.CreateImpl(IMainObject::GetTypeStatic());
        EXPECT_EQ(id, 1);

        ASSERT_GT(factory.GetIdTable().size(), 2);
        IMainObject* object = factory.GetIdTable()[1].object;
        EXPECT_EQ(object->GetType(), IMainObject::GetTypeStatic());
        EXPECT_EQ(object->GetId(), 1);
    }
    {
        quint32 id = factory.CreateImpl(UnsyncGenerator::GetTypeStatic());
        EXPECT_EQ(id, 2);

        ASSERT_GT(factory.GetIdTable().size(), 3);
        IMainObject* object = factory.GetIdTable()[2].object;
        EXPECT_EQ(object->GetType(), UnsyncGenerator::GetTypeStatic());
        EXPECT_EQ(object->GetId(), 2);
    }
    {
        quint32 id = factory.CreateImpl(CubeTile::GetTypeStatic());
        EXPECT_EQ(id, 3);

        ASSERT_GT(factory.GetIdTable().size(), 4);
        IMainObject* object = factory.GetIdTable()[3].object;
        ASSERT_EQ(object->GetType(), CubeTile::GetTypeStatic());
        EXPECT_EQ(object->GetId(), 3);
        CubeTile* tile = static_cast<CubeTile*>(object);
        tile->SetPos(0, 0);

        EXPECT_CALL(game, GetMap())
            .WillOnce(ReturnRef(map));
        EXPECT_CALL(map, GetAtmosphere())
            .WillOnce(ReturnRef(atmos));
        EXPECT_CALL(atmos, SetFlags(0, 0, 0, '\0'));
        quint32 id2 = factory.CreateImpl(IOnMapObject::GetTypeStatic(), id);
        EXPECT_EQ(id2, 4);
        {
            ASSERT_GT(factory.GetIdTable().size(), 5);
            IMainObject* object = factory.GetIdTable()[4].object;
            ASSERT_EQ(object->GetType(), IOnMapObject::GetTypeStatic());
            EXPECT_EQ(object->GetId(), 4);
            IOnMapObject* on_map_object = static_cast<IOnMapObject*>(object);
            EXPECT_EQ(on_map_object->GetOwner().Id(), id);
        }

        EXPECT_CALL(game, GetMap())
            .WillOnce(ReturnRef(map));
        EXPECT_CALL(map, GetAtmosphere())
            .WillOnce(ReturnRef(atmos));
        EXPECT_CALL(atmos, SetFlags(0, 0, 0, '\0'));
        quint32 id3 = factory.CreateImpl(ITurf::GetTypeStatic(), id);
        EXPECT_EQ(id3, 5);
        {
            ASSERT_GT(factory.GetIdTable().size(), 6);
            IMainObject* object = factory.GetIdTable()[5].object;
            ASSERT_EQ(object->GetType(), ITurf::GetTypeStatic());
            EXPECT_EQ(object->GetId(), 5);
            ITurf* turf = static_cast<ITurf*>(object);
            EXPECT_EQ(turf->GetOwner().Id(), id);
        }
    }
}

TEST(ObjectFactory, CreateImplResizeTable)
{
    MockIGame game;
    ObjectFactory factory(&game);

    for (int i = 1; i < 1000; ++i)
    {
        quint32 id = factory.CreateImpl(IMainObject::GetTypeStatic());
        EXPECT_EQ(id, i);
    }
}

TEST(ObjectFactory, AfterWorldCreation)
{
    MockIGame game;
    ObjectFactory factory(&game);

    {
        factory.BeginWorldCreation();

        quint32 id = factory.CreateImpl(TestMainObject::GetTypeStatic());
        ASSERT_EQ(id, 1);

        ASSERT_GT(factory.GetIdTable().size(), 2);
        IMainObject* object = factory.GetIdTable()[1].object;
        ASSERT_EQ(object->GetType(), TestMainObject::GetTypeStatic());

        TestMainObject* test_object = static_cast<TestMainObject*>(object);
        ASSERT_EQ(test_object->after_world_creation_, 0);

        factory.FinishWorldCreation();
        ASSERT_EQ(test_object->after_world_creation_, 1);
    }
    {
        quint32 id = factory.CreateImpl(TestMainObject::GetTypeStatic());
        ASSERT_EQ(id, 2);

        ASSERT_GT(factory.GetIdTable().size(), 3);
        IMainObject* object = factory.GetIdTable()[2].object;
        ASSERT_EQ(object->GetType(), TestMainObject::GetTypeStatic());

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
        quint32 id = factory.CreateImpl(TestMainObject::GetTypeStatic());
        ASSERT_EQ(id, 1);
        ASSERT_GT(factory.GetIdTable().size(), 2);
        IMainObject* object = factory.GetIdTable()[1].object;
        ASSERT_EQ(object->GetType(), TestMainObject::GetTypeStatic());

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

        quint32 id2 = factory.CreateImpl(TestMainObject::GetTypeStatic());
        ASSERT_EQ(id2, 2);
        ASSERT_GT(factory.GetIdTable().size(), 3);
        IMainObject* object2 = factory.GetIdTable()[2].object;
        ASSERT_EQ(object2->GetType(), TestMainObject::GetTypeStatic());

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
        quint32 id = factory.CreateImpl(TestMainObject::GetTypeStatic());
        ASSERT_EQ(id, 1);
        ASSERT_GT(factory.GetIdTable().size(), 2);
        IMainObject* object = factory.GetIdTable()[1].object;
        ASSERT_EQ(object->GetType(), TestMainObject::GetTypeStatic());

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

TEST(ObjectFactory, Hash)
{
    MockIGame game;
    ObjectFactory factory(&game);

    EXPECT_EQ(factory.Hash(), 0);

    TestMainObject* test_object = nullptr;
    {
        quint32 id = factory.CreateImpl(TestMainObject::GetTypeStatic());
        ASSERT_EQ(id, 1);
        ASSERT_GT(factory.GetIdTable().size(), 2);
        IMainObject* object = factory.GetIdTable()[1].object;
        ASSERT_EQ(object->GetType(), TestMainObject::GetTypeStatic());

        test_object = static_cast<TestMainObject*>(object);
    }
    EXPECT_CALL(game, GetFactory())
        .WillOnce(ReturnRef(factory));
    test_object->SetFreq(1);
    EXPECT_EQ(factory.Hash(), 2);

    factory.ForeachProcess();
    EXPECT_EQ(factory.Hash(), 4);

    test_object->SetFreq(0);
    EXPECT_EQ(factory.Hash(), 2);

    EXPECT_CALL(game, GetFactory())
        .WillOnce(ReturnRef(factory));
    test_object->SetFreq(1);
    factory.ForeachProcess();
    EXPECT_EQ(factory.Hash(), 5);

    factory.DeleteLater(test_object->GetId());
    factory.ProcessDeletion();
    EXPECT_EQ(factory.Hash(), 0);
}

TEST(ObjectFactory, SaveAndLoadNoObjects)
{
    FastSerializer serializer;
    quint32 hash = 0;
    {
        MockIGame game;
        MockIMapMaster map;
        SyncRandom rand;
        rand.SetRand(4242, 32);
        ObjectFactory factory(&game);

        hash = factory.Hash();

        EXPECT_CALL(game, GetRandom())
            .WillOnce(ReturnRef(rand))
            .WillOnce(ReturnRef(rand));
        EXPECT_CALL(game, GetMap())
            .WillOnce(ReturnRef(map))
            .WillOnce(ReturnRef(map))
            .WillOnce(ReturnRef(map));

        EXPECT_CALL(map, GetWidth())
            .WillOnce(Return(13));
        EXPECT_CALL(map, GetHeight())
            .WillOnce(Return(17));
        EXPECT_CALL(map, GetDepth())
            .WillOnce(Return(23));

        factory.Save(serializer);
    }
    FastDeserializer deserializer(serializer.GetData(), serializer.GetIndex());
    {
        MockIGame game;
        MockIMapMaster map;
        SyncRandom rand;
        MockIAtmosphere atmos;
        ObjectFactory factory(&game);

        EXPECT_CALL(game, GetRandom())
            .WillOnce(ReturnRef(rand));
        EXPECT_CALL(game, GetMap())
            .WillOnce(ReturnRef(map))
            .WillOnce(ReturnRef(map));
        EXPECT_CALL(map, ResizeMap(13, 17, 23));
        EXPECT_CALL(game, SetMob(0));

        IdPtr<IMob> mob = 0;
        EXPECT_CALL(game, ChangeMob(mob));

        EXPECT_CALL(map, GetAtmosphere())
            .WillOnce(ReturnRef(atmos));
        EXPECT_CALL(atmos, LoadGrid())
            .Times(1);

        factory.Load(deserializer, 0);
        EXPECT_EQ(rand.GetCallsCounter(), 32);
        EXPECT_EQ(rand.GetSeed(), 4242);

        EXPECT_EQ(factory.Hash(), hash);
    }
}

TEST(ObjectFactory, SaveAndLoadWithObjects)
{
    FastSerializer serializer;
    quint32 hash = 0;
    {
        MockIGame game;
        MockIMapMaster map;
        SyncRandom rand;
        rand.SetRand(4242, 32);
        ObjectFactory factory(&game);

        factory.CreateImpl(TestMainObject::GetTypeStatic());
        factory.CreateImpl(TestMainObject::GetTypeStatic());

        factory.SetPlayerId(1234, 2);

        hash = factory.Hash();

        EXPECT_CALL(game, GetRandom())
            .WillOnce(ReturnRef(rand))
            .WillOnce(ReturnRef(rand));
        EXPECT_CALL(game, GetMap())
            .WillOnce(ReturnRef(map))
            .WillOnce(ReturnRef(map))
            .WillOnce(ReturnRef(map));

        EXPECT_CALL(map, GetWidth())
            .WillOnce(Return(13));
        EXPECT_CALL(map, GetHeight())
            .WillOnce(Return(17));
        EXPECT_CALL(map, GetDepth())
            .WillOnce(Return(23));

        factory.Save(serializer);
    }
    FastDeserializer deserializer(serializer.GetData(), serializer.GetIndex());
    {
        MockIGame game;
        MockIMapMaster map;
        SyncRandom rand;
        MockIAtmosphere atmos;
        ObjectFactory factory(&game);

        EXPECT_CALL(game, GetRandom())
            .WillOnce(ReturnRef(rand));
        EXPECT_CALL(game, GetMap())
            .WillOnce(ReturnRef(map))
            .WillOnce(ReturnRef(map));
        EXPECT_CALL(map, ResizeMap(13, 17, 23));
        EXPECT_CALL(game, SetMob(0));

        IdPtr<IMob> mob = 0;
        EXPECT_CALL(game, ChangeMob(mob));

        EXPECT_CALL(map, GetAtmosphere())
            .WillOnce(ReturnRef(atmos));
        EXPECT_CALL(atmos, LoadGrid())
            .Times(1);

        factory.Load(deserializer, 0);
        EXPECT_EQ(rand.GetCallsCounter(), 32);
        EXPECT_EQ(rand.GetSeed(), 4242);

        {
            ASSERT_GE(factory.GetIdTable().size(), 2);
            IMainObject* object = factory.GetIdTable()[1].object;
            ASSERT_TRUE(object);
            ASSERT_EQ(object->GetType(), TestMainObject::GetTypeStatic());
            TestMainObject* test_object = static_cast<TestMainObject*>(object);
            test_object->SetDestructorCallback([](){});
        }
        {
            ASSERT_GE(factory.GetIdTable().size(), 3);
            IMainObject* object = factory.GetIdTable()[2].object;
            ASSERT_TRUE(object);
            ASSERT_EQ(object->GetType(), TestMainObject::GetTypeStatic());
            TestMainObject* test_object = static_cast<TestMainObject*>(object);
            test_object->SetDestructorCallback([](){});
        }

        EXPECT_EQ(factory.Hash(), hash);
        EXPECT_EQ(factory.GetNetId(2), 1234);
        EXPECT_EQ(factory.GetPlayerId(1234), 2);
    }
}

