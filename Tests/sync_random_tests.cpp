#include <gtest/gtest.h>

#include "core/SyncRandom.h"

TEST(SyncRandom, Basics)
{
    SyncRandom random;
    random.SetRand(42, 33033);
    ASSERT_EQ(random.GetSeed(), 42);
    ASSERT_EQ(random.GetCallsCounter(), 33033);
    ASSERT_EQ(random.GetRand(), 2525021);
    ASSERT_EQ(random.GetCallsCounter(), 33034);
    ASSERT_EQ(random.GetRand(), 1242097);
    ASSERT_EQ(random.GetCallsCounter(), 33035);
}
