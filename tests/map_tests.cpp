#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "core/Map.h"

#include "interfaces_mocks.h"

using ::testing::ReturnRef;
using ::testing::Return;

TEST(Map, Constructor)
{
    Map map;
    EXPECT_EQ(map.GetWidth(), 0);
    EXPECT_EQ(map.GetHeight(), 0);
    EXPECT_EQ(map.GetDepth(), 0);
}

TEST(MapDeathTest, IncorrectResize)
{
    Map map;
    EXPECT_DEATH(
    {
        map.Resize(0, 10, 10);
    }, "Incorrect map new size");
    EXPECT_DEATH(
    {
        map.Resize(10, 0, 10);
    }, "Incorrect map new size");
    EXPECT_DEATH(
    {
        map.Resize(10, 10, 0);
    }, "Incorrect map new size");
}

TEST(Map, Resize)
{
    Map map;

    map.Resize(11, 12, 13);
    ASSERT_EQ(map.GetWidth(), 11);
    ASSERT_EQ(map.GetHeight(), 12);
    ASSERT_EQ(map.GetDepth(), 13);

    for (int x = 0; x < 11; ++x)
    {
        for (int y = 0; y < 12; ++y)
        {
            for (int z = 0; z < 13; ++z)
            {
                EXPECT_EQ(map.At(x, y, z).Id(), 0);
            }
        }
    }
}

