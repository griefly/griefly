#include <gtest/gtest.h>

#include "interfaces_mocks.h"

#include "core/ObjectFactory.h"

#include "core/objects/Object.h"
#include "core/objects/Tile.h"
#include "core/objects/MaterialObject.h"
#include "core/objects/turfs/Turf.h"
#include "core/objects/mobs/Mob.h"
#include "core/objects/test/TestObject.h"
#include "core/objects/test/UnsyncGenerator.h"
#include "core/objects/GlobalObjectsHolder.h"

//#include "core/Game.h"

using ::testing::ReturnRef;
using ::testing::Return;

using namespace kv;

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
        quint32 id = factory.CreateImpl(MapObject::GetTypeStatic());
        factory.CreateImpl(MapObject::GetTypeStatic(), id);
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
    MockIAtmosphere atmos;
    ObjectFactory factory(&game);
    {
        quint32 id = factory.CreateImpl(kv::Object::GetTypeStatic());
        EXPECT_EQ(id, 1);

        ASSERT_GT(factory.GetIdTable().size(), 2);
        kv::Object* object = factory.GetIdTable()[1].object;
        EXPECT_EQ(object->GetType(), kv::Object::GetTypeStatic());
        EXPECT_EQ(object->GetId(), 1);
    }
    {
        quint32 id = factory.CreateImpl(UnsyncGenerator::GetTypeStatic());
        EXPECT_EQ(id, 2);

        ASSERT_GT(factory.GetIdTable().size(), 3);
        kv::Object* object = factory.GetIdTable()[2].object;
        EXPECT_EQ(object->GetType(), UnsyncGenerator::GetTypeStatic());
        EXPECT_EQ(object->GetId(), 2);
    }
    {
        quint32 id = factory.CreateImpl(CubeTile::GetTypeStatic());
        EXPECT_EQ(id, 3);

        ASSERT_GT(factory.GetIdTable().size(), 4);
        kv::Object* object = factory.GetIdTable()[3].object;
        ASSERT_EQ(object->GetType(), CubeTile::GetTypeStatic());
        EXPECT_EQ(object->GetId(), 3);
        CubeTile* tile = static_cast<CubeTile*>(object);
        tile->SetPos({0, 0, 0});

        EXPECT_CALL(game, GetAtmosphere())
            .WillOnce(ReturnRef(atmos));
        EXPECT_CALL(atmos, SetFlags(0, 0, 0, '\0'));
        quint32 id2 = factory.CreateImpl(MaterialObject::GetTypeStatic(), id);
        EXPECT_EQ(id2, 4);
        {
            ASSERT_GT(factory.GetIdTable().size(), 5);
            kv::Object* object = factory.GetIdTable()[4].object;
            ASSERT_EQ(object->GetType(), MaterialObject::GetTypeStatic());
            EXPECT_EQ(object->GetId(), 4);
            MaterialObject* on_map_object = static_cast<MaterialObject*>(object);
            EXPECT_EQ(on_map_object->GetOwner().Id(), id);
        }

        EXPECT_CALL(game, GetAtmosphere())
            .WillOnce(ReturnRef(atmos));
        EXPECT_CALL(atmos, SetFlags(0, 0, 0, '\0'));
        quint32 id3 = factory.CreateImpl(Turf::GetTypeStatic(), id);
        EXPECT_EQ(id3, 5);
        {
            ASSERT_GT(factory.GetIdTable().size(), 6);
            kv::Object* object = factory.GetIdTable()[5].object;
            ASSERT_EQ(object->GetType(), Turf::GetTypeStatic());
            EXPECT_EQ(object->GetId(), 5);
            Turf* turf = static_cast<Turf*>(object);
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
        quint32 id = factory.CreateImpl(kv::Object::GetTypeStatic());
        EXPECT_EQ(id, i);
    }
}

TEST(ObjectFactory, AfterWorldCreation)
{
    MockIGame game;
    ObjectFactory factory(&game);

    {
        factory.BeginWorldCreation();

        quint32 id = factory.CreateImpl(TestObject::GetTypeStatic());
        ASSERT_EQ(id, 1);

        ASSERT_GT(factory.GetIdTable().size(), 2);
        kv::Object* object = factory.GetIdTable()[1].object;
        ASSERT_EQ(object->GetType(), TestObject::GetTypeStatic());

        TestObject* test_object = static_cast<TestObject*>(object);
        ASSERT_EQ(test_object->after_world_creation_, 0);

        factory.FinishWorldCreation();
        ASSERT_EQ(test_object->after_world_creation_, 1);
    }
    {
        quint32 id = factory.CreateImpl(TestObject::GetTypeStatic());
        ASSERT_EQ(id, 2);

        ASSERT_GT(factory.GetIdTable().size(), 3);
        kv::Object* object = factory.GetIdTable()[2].object;
        ASSERT_EQ(object->GetType(), TestObject::GetTypeStatic());

        TestObject* test_object = static_cast<TestObject*>(object);
        ASSERT_EQ(test_object->after_world_creation_, 1);
    }
}

TEST(ObjectFactory, Process)
{
    MockIGame game;
    ObjectFactory factory(&game);
    factory.FinishWorldCreation();

    {
        quint32 id = factory.CreateImpl(TestObject::GetTypeStatic());
        ASSERT_EQ(id, 1);
        ASSERT_GT(factory.GetIdTable().size(), 2);
        kv::Object* object = factory.GetIdTable()[1].object;
        ASSERT_EQ(object->GetType(), TestObject::GetTypeStatic());

        TestObject* test_object = static_cast<TestObject*>(object);
        EXPECT_EQ(test_object->process_, 0);

        IdPtr<GlobalObjectsHolder> globals = factory.CreateImpl(GlobalObjectsHolder::GetTypeStatic());
        globals->game_tick = 1;

        EXPECT_CALL(game, GetGlobals())
            .WillRepeatedly(Return(globals));

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

        globals->game_tick = 100;
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
            .WillRepeatedly(ReturnRef(factory));
        test_object->SetFreq(1);
        factory.GetIdTable()[1].object = nullptr;
        factory.ForeachProcess();
        EXPECT_EQ(test_object->process_, 4);
        factory.GetIdTable()[1].object = object;
        test_object->SetFreq(1);

        quint32 id2 = factory.CreateImpl(TestObject::GetTypeStatic());
        ASSERT_EQ(id2, 3);
        ASSERT_GT(factory.GetIdTable().size(), 4);
        kv::Object* object2 = factory.GetIdTable()[3].object;
        ASSERT_EQ(object2->GetType(), TestObject::GetTypeStatic());

        TestObject* test_object2 = static_cast<TestObject*>(object2);
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
            factory.GetIdTable()[3].object = nullptr;
        });
        EXPECT_CALL(game, GetFactory())
            .WillOnce(ReturnRef(factory));
        object2->SetFreq(1);
        factory.ForeachProcess();
        EXPECT_EQ(test_object2->process_, 0);
        factory.GetIdTable()[3].object = object2;

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
        quint32 id = factory.CreateImpl(TestObject::GetTypeStatic());
        ASSERT_EQ(id, 1);
        ASSERT_GT(factory.GetIdTable().size(), 2);
        kv::Object* object = factory.GetIdTable()[1].object;
        ASSERT_EQ(object->GetType(), TestObject::GetTypeStatic());

        TestObject* test_object = static_cast<TestObject*>(object);
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

TEST(ObjectFactory, Hash)
{
    MockIGame game;
    ObjectFactory factory(&game);

    EXPECT_EQ(factory.Hash(), 0);

    IdPtr<GlobalObjectsHolder> globals = factory.CreateImpl(GlobalObjectsHolder::GetTypeStatic());
    globals->game_tick = 1;
    EXPECT_CALL(game, GetGlobals())
        .WillRepeatedly(Return(globals));


    TestObject* test_object = nullptr;
    {
        quint32 id = factory.CreateImpl(TestObject::GetTypeStatic());
        ASSERT_EQ(id, 2);
        ASSERT_GT(factory.GetIdTable().size(), 3);
        kv::Object* object = factory.GetIdTable()[2].object;
        ASSERT_EQ(object->GetType(), TestObject::GetTypeStatic());

        test_object = static_cast<TestObject*>(object);
    }
    EXPECT_CALL(game, GetFactory())
        .WillOnce(ReturnRef(factory));
    test_object->SetFreq(1);
    EXPECT_EQ(factory.Hash(), 5);

    factory.ForeachProcess();
    EXPECT_EQ(factory.Hash(), 8);

    test_object->SetFreq(0);
    EXPECT_EQ(factory.Hash(), 7);

    EXPECT_CALL(game, GetFactory())
        .WillOnce(ReturnRef(factory));
    test_object->SetFreq(1);
    factory.ForeachProcess();
    EXPECT_EQ(factory.Hash(), 9);

    factory.DeleteLater(test_object->GetId());
    factory.ProcessDeletion();
    EXPECT_EQ(factory.Hash(), 4);
}

TEST(ObjectFactory, Hearer)
{
    MockIGame game;
    ObjectFactory factory(&game);

    IdPtr<GlobalObjectsHolder> globals
        = factory.CreateImpl(GlobalObjectsHolder::GetTypeStatic());

    EXPECT_CALL(game, GetGlobals())
        .WillRepeatedly(Return(globals));

    EXPECT_EQ(globals->hearers.size(), 0);

    IdPtr<Object> hearer = factory.CreateImpl(TestHearer::GetTypeStatic());

    ASSERT_EQ(globals->hearers.size(), 1);
    EXPECT_EQ(globals->hearers[0], hearer);
}
