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

TEST(SyncRandom, Pick)
{
    SyncRandom random;
    random.SetRand(123456, 3);

    std::vector<int> ints;
    ints.push_back(1);
    ints.push_back(2);
    ints.push_back(3);
    ints.push_back(4);
    ints.push_back(5);
    int pickval = random.Pick(ints);
    ASSERT_EQ(pickval, 2);
    pickval = random.Pick(ints);
    ASSERT_EQ(pickval, 3);
    pickval = random.Pick(ints);
    ASSERT_EQ(pickval, 4);
    pickval = random.Pick(ints);
    ASSERT_EQ(pickval, 1);
}
