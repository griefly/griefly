#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "core/WorldLoaderSaver.h"
#include "core/ObjectFactory.h"
#include "core/objects/test/TestMainObject.h"

#include "interfaces_mocks.h"

using ::testing::ReturnRef;
using ::testing::Return;

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

        MockIGame::PlayersTable table;
        table[1] = 1;
        EXPECT_CALL(game, GetPlayersTable())
            .WillRepeatedly(ReturnRef(table));
        EXPECT_CALL(game, GetRandom())
            .WillRepeatedly(ReturnRef(rand));
        EXPECT_CALL(game, GetMap())
            .WillRepeatedly(ReturnRef(map));

        EXPECT_CALL(map, GetWidth())
            .WillOnce(Return(13));
        EXPECT_CALL(map, GetHeight())
            .WillOnce(Return(17));
        EXPECT_CALL(map, GetDepth())
            .WillOnce(Return(23));

        EXPECT_CALL(game, GetFactory())
            .WillRepeatedly(ReturnRef(factory));

        WorldLoaderSaver loader_saver(&game);
        loader_saver.Save(serializer);
    }
    FastDeserializer deserializer(serializer.GetData(), serializer.GetIndex());
    {
        MockIGame game;
        MockIMapMaster map;
        SyncRandom rand;
        MockIAtmosphere atmos;
        ObjectFactory factory(&game);

        EXPECT_CALL(game, SetPlayerId(1, 1));
        EXPECT_CALL(game, GetPlayerId(0))
            .WillOnce(Return(0));
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

        EXPECT_CALL(game, GetFactory())
            .WillRepeatedly(ReturnRef(factory));

        WorldLoaderSaver loader_saver(&game);
        loader_saver.Load(deserializer, 0);
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

        hash = factory.Hash();

        MockIGame::PlayersTable table;
        table[1] = 1;

        EXPECT_CALL(game, GetFactory())
            .WillRepeatedly(ReturnRef(factory));
        EXPECT_CALL(game, GetPlayersTable())
            .WillOnce(ReturnRef(table));
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

        WorldLoaderSaver loader_saver(&game);
        loader_saver.Save(serializer);
    }
    FastDeserializer deserializer(serializer.GetData(), serializer.GetIndex());
    {
        MockIGame game;
        MockIMapMaster map;
        SyncRandom rand;
        MockIAtmosphere atmos;
        ObjectFactory factory(&game);

        EXPECT_CALL(game, GetFactory())
            .WillRepeatedly(ReturnRef(factory));
        EXPECT_CALL(game, SetPlayerId(1, 1));
        EXPECT_CALL(game, GetPlayerId(0))
            .WillOnce(Return(0));
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

        WorldLoaderSaver loader_saver(&game);
        loader_saver.Load(deserializer, 0);
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
    }
}
