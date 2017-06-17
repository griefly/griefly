#include <gtest/gtest.h>

#include "core/SyncRandom.h"

using namespace kv;

TEST(SynchronizedRandom, Basics)
{
    SynchronizedRandom random;

    for (int i = 0; i < 16; ++i)
    {
        random.SetParams(42, 33033);
        EXPECT_EQ(random.GetSeed(), 42);
        EXPECT_EQ(random.GetCallsCounter(), 33033);
        EXPECT_EQ(random.Generate(), 2525021);
        EXPECT_EQ(random.GetCallsCounter(), 33034);
        EXPECT_EQ(random.Generate(), 1242097);
        EXPECT_EQ(random.GetCallsCounter(), 33035);
    }

    random.SetParams(100, 1);
    EXPECT_EQ(random.GetSeed(), 100);
    EXPECT_EQ(random.GetCallsCounter(), 1);
    EXPECT_EQ(random.Generate(), 4999187);
    EXPECT_EQ(random.GetCallsCounter(), 2);
    EXPECT_EQ(random.Generate(), 4098804);
    EXPECT_EQ(random.GetCallsCounter(), 3);
}
