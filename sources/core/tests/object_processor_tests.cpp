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

    IdPtr<TestObject> object = factory_.CreateImpl(TestObject::GetTypeStatic());
    int value1 = 0;
    object->SetProcessCallback([&value1]() { ++value1; });
    object->SetFreq(1);
    processor->RunProcess();
    EXPECT_EQ(value1, 0);

    // TODO (?): remove all Add cause SetFreq will call them anyway
    processor->Add(object.Id());
    EXPECT_EQ(processor->HashMembers(), 5);
    processor->Add(object.Id());
    EXPECT_EQ(processor->HashMembers(), 5);

    processor->RunProcess();
    EXPECT_EQ(value1, 1);
    processor->RunProcess();
    EXPECT_EQ(value1, 2);
    EXPECT_EQ(processor->HashMembers(), 5);

    object->SetFreq(0);
    processor->RunProcess();
    EXPECT_EQ(processor->HashMembers(), 2);
}

TEST_F(ObjectProcessorTest, AddDuringProcess)
{
    IdPtr<ObjectProcessor> processor = factory_.CreateImpl(ObjectProcessor::GetTypeStatic());

    IdPtr<TestObject> object1 = factory_.CreateImpl(TestObject::GetTypeStatic());
    IdPtr<TestObject> object2 = factory_.CreateImpl(TestObject::GetTypeStatic());
    int value1 = 0;
    int value2 = 0;
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

    processor->RunProcess();
    EXPECT_EQ(value1, 1);
    EXPECT_EQ(value2, 0);

    processor->RunProcess();
    EXPECT_EQ(value1, 2);
    EXPECT_EQ(value2, 1);

    processor->RunProcess();
    EXPECT_EQ(value1, 3);
    EXPECT_EQ(value2, 2);
}
