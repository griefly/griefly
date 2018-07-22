#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Mapgen.h>

#include "WorldLoaderSaver.h"
#include "ObjectFactory.h"
#include "objects/test/TestObject.h"
#include "objects/GlobalObjectsHolder.h"

#include "interfaces_mocks.h"

using ::testing::ReturnRef;
using ::testing::Return;
using ::testing::Const;

using namespace kv;

TEST(WorldLoaderSaver, SaveAndLoadNoObjects)
{
    FastSerializer serializer;
    quint32 hash = 0;
    {
        MockIGame game;
        ObjectFactory factory(&game);

        hash = factory.Hash();

        IdPtr<kv::GlobalObjectsHolder> globals = 99;
        EXPECT_CALL(game, GetGlobals())
            .WillRepeatedly(Return(globals));

        EXPECT_CALL(game, GetFactory())
            .WillRepeatedly(ReturnRef(factory));
        EXPECT_CALL(Const(game), GetFactory())
            .WillRepeatedly(ReturnRef(factory));

        world::Save(&game, serializer);
    }
    FastDeserializer deserializer(serializer.GetData(), serializer.GetIndex());
    {
        MockIGame game;
        MockIMap map;
        MockIAtmosphere atmos;
        ObjectFactory factory(&game);

        EXPECT_CALL(game, SetGlobals(99));
        EXPECT_CALL(game, GetMap())
            .WillRepeatedly(ReturnRef(map));

        EXPECT_CALL(game, GetAtmosphere())
            .WillRepeatedly(ReturnRef(atmos));
        EXPECT_CALL(atmos, LoadGrid(&map))
            .Times(1);

        EXPECT_CALL(game, GetFactory())
            .WillRepeatedly(ReturnRef(factory));
        EXPECT_CALL(Const(game), GetFactory())
            .WillRepeatedly(ReturnRef(factory));

        world::Load(&game, deserializer);

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
        ObjectFactory factory(&game);

        factory.CreateImpl(TestObject::GetTypeStatic());
        factory.CreateImpl(TestObject::GetTypeStatic());
        IdPtr<GlobalObjectsHolder> globals
            = factory.CreateImpl(GlobalObjectsHolder::GetTypeStatic());;
        globals_id = globals.Id();

        hash = factory.Hash();

        EXPECT_CALL(game, GetFactory())
            .WillRepeatedly(ReturnRef(factory));
        EXPECT_CALL(Const(game), GetFactory())
            .WillRepeatedly(ReturnRef(factory));
        EXPECT_CALL(game, GetMap())
            .WillRepeatedly(ReturnRef(map));
        EXPECT_CALL(game, GetGlobals())
            .WillRepeatedly(Return(globals));

        world::Save(&game, serializer);
    }
    FastDeserializer deserializer(serializer.GetData(), serializer.GetIndex());
    {
        MockIGame game;
        MockIMap map;
        MockIAtmosphere atmos;
        ObjectFactory factory(&game);

        EXPECT_CALL(game, GetFactory())
            .WillRepeatedly(ReturnRef(factory));
        EXPECT_CALL(Const(game), GetFactory())
            .WillRepeatedly(ReturnRef(factory));
        EXPECT_CALL(game, SetGlobals(globals_id));
        EXPECT_CALL(game, GetMap())
            .WillRepeatedly(ReturnRef(map));

        EXPECT_CALL(game, GetAtmosphere())
            .WillRepeatedly(ReturnRef(atmos));
        EXPECT_CALL(atmos, LoadGrid(&map))
            .Times(1);

        world::Load(&game, deserializer);

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

TEST(WorldLoaderSaver, ConvertJsonToSerialized)
{
    // Unknown
    EXPECT_EQ(world::ConvertJsonToSerialized(
        QJsonObject{{"test", "test"}}), QByteArray());

    // Bool
    EXPECT_EQ(world::ConvertJsonToSerialized(
        QJsonObject{{mapgen::key::type::BOOL, true}}), QByteArray("\x01\x01"));
    EXPECT_EQ(world::ConvertJsonToSerialized(
        QJsonObject{{mapgen::key::type::BOOL, false}}), QByteArray("\x01\x00", 2));

    // Int32
    EXPECT_EQ(world::ConvertJsonToSerialized(
        QJsonObject{{mapgen::key::type::INT32, 0}}),
        QByteArray("\x02\x00\x00\x00\x00", 5));
    EXPECT_EQ(world::ConvertJsonToSerialized(
        QJsonObject{{mapgen::key::type::INT32, 42}}),
        QByteArray("\x02\x2A\x00\x00\x00", 5));
    EXPECT_EQ(world::ConvertJsonToSerialized(
        QJsonObject{{mapgen::key::type::INT32, std::numeric_limits<qint32>::max()}}),
        QByteArray("\x02\xFF\xFF\xFF\x7F", 5));
    EXPECT_EQ(world::ConvertJsonToSerialized(
        QJsonObject{{mapgen::key::type::INT32, std::numeric_limits<qint32>::min()}}),
        QByteArray("\x02\x00\x00\x00\x80", 5));

    // UInt32
    EXPECT_EQ(world::ConvertJsonToSerialized(
        QJsonObject{{mapgen::key::type::UINT32, 0}}),
        QByteArray("\x03\x00\x00\x00\x00", 5));
    EXPECT_EQ(world::ConvertJsonToSerialized(
        QJsonObject{{mapgen::key::type::UINT32, 42}}),
        QByteArray("\x03\x2A\x00\x00\x00", 5));
    EXPECT_EQ(world::ConvertJsonToSerialized(
        QJsonObject{{mapgen::key::type::UINT32, static_cast<double>(std::numeric_limits<quint32>::max())}}),
        QByteArray("\x03\xFF\xFF\xFF\xFF", 5));

    // String
    EXPECT_EQ(world::ConvertJsonToSerialized(
        QJsonObject{{mapgen::key::type::STRING, ""}}),
        QByteArray("\x04\x02\x00\x00\x00\x00", 6));
    EXPECT_EQ(world::ConvertJsonToSerialized(
        QJsonObject{{mapgen::key::type::STRING, "cya"}}),
        QByteArray("\x04\x02\x03\x00\x00\x00\x63\x00\x79\x00\x61\x00", 12));
    EXPECT_EQ(world::ConvertJsonToSerialized(
        QJsonObject{{mapgen::key::type::STRING, "пока"}}),
        QByteArray("\x04\x02\x04\x00\x00\x00\x3F\x04\x3E\x04\x3A\x04\x30\x04", 14));

    // Bytearray
    EXPECT_EQ(world::ConvertJsonToSerialized(
        QJsonObject{{mapgen::key::type::BYTEARRAY, ""}}),
        QByteArray("\x05\x02\x00\x00\x00\x00", 6));
    EXPECT_EQ(world::ConvertJsonToSerialized(
        QJsonObject{{mapgen::key::type::BYTEARRAY, "ff03020100"}}),
        QByteArray("\x05\x02\x05\x00\x00\x00\xFF\x03\x02\x01\x00", 11));

    // Type
    EXPECT_EQ(world::ConvertJsonToSerialized(
        QJsonObject{{mapgen::key::type::TYPE, "type"}}),
        QByteArray("\x06\x02\x04\x00\x00\x00\x74\x00\x79\x00\x70\x00\x65\x00", 14));
}
