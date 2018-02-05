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

TEST(ObjectProcessor, Constructor)
{
    // TODO: proper cast table initialization
    InitCastTable();

    MockIGame game;
    ObjectFactory factory(&game);
    factory.FinishWorldCreation();

    IdPtr<ObjectProcessor> processor = factory.CreateImpl(ObjectProcessor::GetTypeStatic());
    EXPECT_EQ(processor->HashMembers(), 1);
}

TEST(ObjectProcessor, SimpleProcess)
{
    // TODO: proper cast table initialization
    InitCastTable();

    MockIGame game;
    ObjectFactory factory(&game);
    factory.FinishWorldCreation();

    IdPtr<GlobalObjectsHolder> globals = factory.CreateImpl(GlobalObjectsHolder::GetTypeStatic());
    globals->game_tick = 1;

    EXPECT_CALL(game, GetGlobals())
        .WillRepeatedly(Return(globals));

    EXPECT_CALL(game, GetFactory())
        .WillRepeatedly(ReturnRef(factory));

    IdPtr<ObjectProcessor> processor = factory.CreateImpl(ObjectProcessor::GetTypeStatic());

    IdPtr<TestObject> object = factory.CreateImpl(TestObject::GetTypeStatic());
    int value1 = 0;
    object->SetProcessCallback([&value1]() { ++value1; });
    object->SetFreq(1);
    processor->RunProcess();
    EXPECT_EQ(value1, 0);

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
