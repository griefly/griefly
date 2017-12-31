#include <gtest/gtest.h>

#include "Names.h"

#include "SynchronizedRandom.h"
#include "objects/GlobalObjectsHolder.h"

#include "ObjectFactory.h"
#include "interfaces_mocks.h"

using ::testing::Return;

TEST(Names, Basics)
{
    MockIGame game;
    ObjectFactory factory(&game);

    IdPtr<kv::GlobalObjectsHolder> globals = factory.CreateImpl(kv::GlobalObjectsHolder::GetTypeStatic());
    globals->random = factory.CreateImpl(kv::SynchronizedRandom::GetTypeStatic());

    EXPECT_CALL(game, GetGlobals())
        .WillRepeatedly(Return(globals));

    Names names(&game);
    for (int i = 0; i < 20; ++i)
    {
        QString name = names.GetMaleName();
        EXPECT_FALSE(name.isEmpty());
        EXPECT_TRUE(name.contains(' '));
        for (const QChar ch : name)
        {
            EXPECT_NE(ch, QChar('\n'));
        }
    }
}

