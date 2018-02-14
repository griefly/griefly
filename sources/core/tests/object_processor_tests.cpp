#include <gtest/gtest.h>

#include "objects/ObjectProcessor.h"

#include "ObjectFactory.h"
#include "objects/test/TestObject.h"
#include "objects/GlobalObjectsHolder.h"

#include "interfaces_mocks.h"

#include <QDebug>

using namespace kv;

using ::testing::Return;
using ::testing::ReturnRef;

class ObjectProcessorTest : public ::testing::Test
{
public:
    ObjectProcessorTest()
        : factory_(&game_)
    {
        // TODO: proper cast table initialization
        InitCastTable();

        factory_.FinishWorldCreation();

        globals_ = factory_.CreateImpl(GlobalObjectsHolder::GetTypeStatic());
        globals_->game_tick = 1;

        EXPECT_CALL(game_, GetGlobals())
            .WillRepeatedly(Return(globals_));

        EXPECT_CALL(game_, GetFactory())
            .WillRepeatedly(ReturnRef(factory_));
    }
protected:
    MockIGame game_;
    ObjectFactory factory_;
    IdPtr<GlobalObjectsHolder> globals_;
};

TEST_F(ObjectProcessorTest, Constructor)
{
    IdPtr<ObjectProcessor> processor
        = factory_.CreateImpl(ObjectProcessor::GetTypeStatic());
    EXPECT_EQ(processor->HashMembers(), 2);
}

TEST_F(ObjectProcessorTest, SimpleProcess)
{
    IdPtr<ObjectProcessor> processor = factory_.CreateImpl(ObjectProcessor::GetTypeStatic());

    EXPECT_CALL(game_, GetProcessor())
        .WillRepeatedly(ReturnRef(*processor));

    IdPtr<TestObject> object = factory_.CreateImpl(TestObject::GetTypeStatic());
    int value1 = 0;
    object->SetProcessCallback([&value1]() { ++value1; });
    processor->RunProcess();
    EXPECT_EQ(value1, 0);

    // TODO (?): remove all Add cause SetFreq will call them anyway
    processor->Add(object.Id());
    EXPECT_EQ(processor->HashMembers(), 5);
    processor->Add(object.Id());
    EXPECT_EQ(processor->HashMembers(), 5);

    object->SetFreq(1);
    processor->RunProcess();
    EXPECT_EQ(value1, 1);
    processor->RunProcess();
    EXPECT_EQ(value1, 2);
    EXPECT_EQ(processor->HashMembers(), 5);

    object->SetFreq(0);
    processor->RunProcess();
    EXPECT_EQ(value1, 2);
    EXPECT_EQ(processor->HashMembers(), 2);

    object->SetFreq(1);
    processor->Add(object.Id());
    processor->RunProcess();
    EXPECT_EQ(value1, 3);

    factory_.DeleteLater(object.Id());
    processor->RunProcess();
    EXPECT_EQ(value1, 3);
}

TEST_F(ObjectProcessorTest, AddAndRemoveDuringProcess)
{
    IdPtr<ObjectProcessor> processor = factory_.CreateImpl(ObjectProcessor::GetTypeStatic());

    EXPECT_CALL(game_, GetProcessor())
        .WillRepeatedly(ReturnRef(*processor));

    IdPtr<TestObject> object1 = factory_.CreateImpl(TestObject::GetTypeStatic());
    IdPtr<TestObject> object2 = factory_.CreateImpl(TestObject::GetTypeStatic());
    IdPtr<TestObject> object3 = factory_.CreateImpl(TestObject::GetTypeStatic());
    int value1 = 0;
    int value2 = 0;
    int value3 = 0;
    object1->SetProcessCallback(
    [&]()
    {
        ++value1;
        object2->SetFreq(1);
        processor->Add(object2.Id());
    });
    object1->SetFreq(1);
    processor->Add(object1.Id());
    object2->SetProcessCallback(
    [&]()
    {
        ++value2;
    });
    object3->SetProcessCallback(
    [&]()
    {
        ++value3;
    });

    processor->RunProcess();
    EXPECT_EQ(value1, 1);
    EXPECT_EQ(value2, 0);
    EXPECT_EQ(value3, 0);

    processor->RunProcess();
    EXPECT_EQ(value1, 2);
    EXPECT_EQ(value2, 1);
    EXPECT_EQ(value3, 0);

    processor->RunProcess();
    EXPECT_EQ(value1, 3);
    EXPECT_EQ(value2, 2);
    EXPECT_EQ(value3, 0);

    object3->SetFreq(1);
    processor->Add(object3.Id());

    processor->RunProcess();
    EXPECT_EQ(value1, 4);
    EXPECT_EQ(value2, 3);
    EXPECT_EQ(value3, 1);

    object2->SetProcessCallback(
    [&]()
    {
        ++value2;
        object1->SetFreq(0);
        object2->SetFreq(0);
        object3->SetFreq(0);
    });

    processor->RunProcess();
    EXPECT_EQ(value1, 5);
    EXPECT_EQ(value2, 4);
    EXPECT_EQ(value3, 1);

    processor->RunProcess();
    EXPECT_EQ(value1, 5);
    EXPECT_EQ(value2, 4);
    EXPECT_EQ(value3, 1);
}

namespace
{

const QByteArray SAVED_DATA
    = QByteArray(
        "\x06\x02\x0F\x00\x00\x00\x4F\x00\x62\x00\x6A\x00\x65\x00\x63"
        "\x00\x74\x00\x50\x00\x72\x00\x6F\x00\x63\x00\x65\x00\x73\x00"
        "\x73\x00\x6F\x00\x72\x00\x03\x02\x00\x00\x00\x02\x00\x00\x00"
        "\x00\x02\x03\x00\x00\x00\x03\x03\x00\x00\x00\x03\x04\x00\x00"
        "\x00\x03\x05\x00\x00\x00", 66);

}

TEST_F(ObjectProcessorTest, Save)
{
    IdPtr<ObjectProcessor> processor = factory_.CreateImpl(ObjectProcessor::GetTypeStatic());

    IdPtr<TestObject> object1 = factory_.CreateImpl(TestObject::GetTypeStatic());
    IdPtr<TestObject> object2 = factory_.CreateImpl(TestObject::GetTypeStatic());
    IdPtr<TestObject> object3 = factory_.CreateImpl(TestObject::GetTypeStatic());

    processor->Add(object1.Id());
    processor->Add(object2.Id());
    processor->Add(object3.Id());

    EXPECT_EQ(processor->HashMembers(), 28);

    kv::FastSerializer serializer(1);
    processor->Save(serializer);

    EXPECT_EQ(
        QByteArray(serializer.GetData(), static_cast<int>(serializer.GetIndex())),
        SAVED_DATA);
}

TEST_F(ObjectProcessorTest, Load)
{
    kv::FastDeserializer deserializer(
        SAVED_DATA.data(), static_cast<quint32>(SAVED_DATA.size()));

    QString type;
    deserializer.ReadType(&type);
    ASSERT_EQ(type, ObjectProcessor::GetTypeStatic());
    quint32 id_loc;
    deserializer >> id_loc;

    factory_.CreateVoid(ObjectProcessor::GetTypeStatic(), id_loc);
    IdPtr<ObjectProcessor> processor = id_loc;

    IdPtr<TestObject> object1 = factory_.CreateImpl(TestObject::GetTypeStatic());
    IdPtr<TestObject> object2 = factory_.CreateImpl(TestObject::GetTypeStatic());
    IdPtr<TestObject> object3 = factory_.CreateImpl(TestObject::GetTypeStatic());

    processor->Load(deserializer);

    EXPECT_EQ(processor->HashMembers(), 28);

    processor->Add(object1.Id());
    processor->Add(object2.Id());
    processor->Add(object3.Id());

    EXPECT_EQ(processor->HashMembers(), 28);
}
