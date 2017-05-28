#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "core/WorldLoaderSaver.h"
#include "core/ObjectFactory.h"
#include "core/objects/test/TestObject.h"
#include "core/objects/GlobalObjectsHolder.h"

#include "interfaces_mocks.h"

using ::testing::ReturnRef;
using ::testing::Return;

using namespace kv;

TEST(WorldLoaderSaver, SaveAndLoadNoObjects)
{
    FastSerializer serializer;
    quint32 hash = 0;
    {
        MockIGame game;
        MockIMap map;
        SyncRandom rand;
        rand.SetRand(4242, 32);
        ObjectFactory factory(&game);

        hash = factory.Hash();

        EXPECT_CALL(game, GetRandom())
            .WillRepeatedly(ReturnRef(rand));
        EXPECT_CALL(game, GetMap())
            .WillRepeatedly(ReturnRef(map));
        IdPtr<kv::GlobalObjectsHolder> globals = 99;
        EXPECT_CALL(game, GetGlobals())
            .WillRepeatedly(Return(globals));

        EXPECT_CALL(game, GetFactory())
            .WillRepeatedly(ReturnRef(factory));

        WorldLoaderSaver loader_saver(&game);
        loader_saver.Save(serializer);
    }
    FastDeserializer deserializer(serializer.GetData(), serializer.GetIndex());
    {
        MockIGame game;
        MockIMap map;
        SyncRandom rand;
        MockIAtmosphere atmos;
        ObjectFactory factory(&game);

        EXPECT_CALL(game, SetGlobals(99));
        EXPECT_CALL(game, GetRandom())
            .WillRepeatedly(ReturnRef(rand));
        EXPECT_CALL(game, GetMap())
            .WillRepeatedly(ReturnRef(map));
        EXPECT_CALL(game, SetMob(0));

        EXPECT_CALL(game, GetAtmosphere())
            .WillRepeatedly(ReturnRef(atmos));
        EXPECT_CALL(atmos, LoadGrid(&map))
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

TEST(WorldLoaderSaver, SaveAndLoadWithObjects)
{
    FastSerializer serializer;
    quint32 hash = 0;
    quint32 globals_id = 0;
    {
        MockIGame game;
        MockIMap map;
        SyncRandom rand;
        rand.SetRand(4242, 32);
        ObjectFactory factory(&game);

        factory.CreateImpl(TestObject::GetTypeStatic());
        factory.CreateImpl(TestObject::GetTypeStatic());
        IdPtr<GlobalObjectsHolder> globals
            = factory.CreateImpl(GlobalObjectsHolder::GetTypeStatic());;
        globals_id = globals.Id();

        hash = factory.Hash();

        EXPECT_CALL(game, GetFactory())
            .WillRepeatedly(ReturnRef(factory));
        EXPECT_CALL(game, GetRandom())
            .WillRepeatedly(ReturnRef(rand));
        EXPECT_CALL(game, GetMap())
            .WillRepeatedly(ReturnRef(map));
        EXPECT_CALL(game, GetGlobals())
            .WillRepeatedly(Return(globals));

        WorldLoaderSaver loader_saver(&game);
        loader_saver.Save(serializer);
    }
    FastDeserializer deserializer(serializer.GetData(), serializer.GetIndex());
    {
        MockIGame game;
        MockIMap map;
        SyncRandom rand;
        MockIAtmosphere atmos;
        ObjectFactory factory(&game);

        EXPECT_CALL(game, GetFactory())
            .WillRepeatedly(ReturnRef(factory));
        EXPECT_CALL(game, SetGlobals(globals_id));
        EXPECT_CALL(game, GetRandom())
            .WillRepeatedly(ReturnRef(rand));
        EXPECT_CALL(game, GetMap())
            .WillRepeatedly(ReturnRef(map));
        EXPECT_CALL(game, SetMob(0));

        EXPECT_CALL(game, GetAtmosphere())
            .WillRepeatedly(ReturnRef(atmos));
        EXPECT_CALL(atmos, LoadGrid(&map))
            .Times(1);

        WorldLoaderSaver loader_saver(&game);
        loader_saver.Load(deserializer, 0);
        EXPECT_EQ(rand.GetCallsCounter(), 32);
        EXPECT_EQ(rand.GetSeed(), 4242);

        {
            ASSERT_GE(factory.GetIdTable().size(), 2);
            Object* object = factory.GetIdTable()[1].object;
            ASSERT_TRUE(object);
            ASSERT_EQ(object->GetType(), TestObject::GetTypeStatic());
            TestObject* test_object = static_cast<TestObject*>(object);
            test_object->SetDestructorCallback([](){});
        }
        {
            ASSERT_GE(factory.GetIdTable().size(), 3);
            Object* object = factory.GetIdTable()[2].object;
            ASSERT_TRUE(object);
            ASSERT_EQ(object->GetType(), TestObject::GetTypeStatic());
            TestObject* test_object = static_cast<TestObject*>(object);
            test_object->SetDestructorCallback([](){});
        }

        EXPECT_EQ(factory.Hash(), hash);
    }
}
