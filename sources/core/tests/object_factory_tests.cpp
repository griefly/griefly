#include <gtest/gtest.h>

#include <Mapgen.h>

#include "interfaces_mocks.h"

#include "ObjectFactory.h"

#include "objects/Object.h"
#include "objects/Tile.h"
#include "objects/MaterialObject.h"
#include "objects/turfs/Turf.h"
#include "objects/mobs/Mob.h"
#include "objects/test/TestObject.h"
#include "objects/test/UnsyncGenerator.h"
#include "objects/GlobalObjectsHolder.h"

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

    // TODO: proper cast table initialization
    InitCastTable();

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

TEST(ObjectFactory, CreateAssetImpl)
{
    // TODO: check why it should be called manually
    // something is off here
    InitSettersForTypes();

    MockIGame game;
    MockIAtmosphere atmos;
    ObjectFactory factory(&game);
    EXPECT_CALL(game, GetFactory())
        .WillRepeatedly(ReturnRef(factory));
    {
        kv::Asset asset;
        asset.sprite = "test sprite";
        asset.state = "test state";
        asset.turf = false;
        asset.type_name = kv::MaterialObject::GetTypeStatic();
        quint32 id = factory.CreateAssetImpl(asset);
        EXPECT_EQ(id, 1);

        ASSERT_GT(factory.GetIdTable().size(), 2);
        kv::Object* object = factory.GetIdTable()[1].object;
        ASSERT_EQ(object->GetType(), kv::MaterialObject::GetTypeStatic());
        EXPECT_EQ(object->GetId(), 1);
        kv::MaterialObject* material_object = static_cast<kv::MaterialObject*>(object);
        EXPECT_EQ(material_object->GetView().GetBaseFrameset().GetSprite(), asset.sprite);
        EXPECT_EQ(material_object->GetView().GetBaseFrameset().GetState(), asset.state);
    }
    {
        kv::Asset asset;
        asset.sprite = "test sprite 2";
        asset.state = "test state 2";
        asset.turf = false;
        asset.type_name = kv::MaterialObject::GetTypeStatic();
        asset.variables.push_back(Asset::VariableInfo{"name_", mapgen::key::type::STRING, QJsonValue("test name")});
        quint32 id = factory.CreateAssetImpl(asset);
        EXPECT_EQ(id, 2);

        ASSERT_GT(factory.GetIdTable().size(), 3);
        kv::Object* object = factory.GetIdTable()[2].object;
        ASSERT_EQ(object->GetType(), kv::MaterialObject::GetTypeStatic());
        EXPECT_EQ(object->GetId(), 2);
        kv::MaterialObject* material_object = static_cast<kv::MaterialObject*>(object);
        EXPECT_EQ(material_object->GetView().GetBaseFrameset().GetSprite(), asset.sprite);
        EXPECT_EQ(material_object->GetView().GetBaseFrameset().GetState(), asset.state);
        qDebug() << material_object->GetName();
        EXPECT_EQ(material_object->GetName(), QString("test name"));
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
    EXPECT_EQ(factory.Hash(), 4);

    factory.DeleteLater(test_object->GetId());
    factory.ProcessDeletion();
    EXPECT_EQ(factory.Hash(), 2);
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
