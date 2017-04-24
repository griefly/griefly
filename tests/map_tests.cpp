#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "core/Map.h"

#include "interfaces_mocks.h"

using ::testing::ReturnRef;
using ::testing::Return;

TEST(Map, Constructor)
{
    // TODO: MapMaster should take 'IAtmosphere' as argument in constructor
    TextPainter text_painter;
    SyncRandom sync_random;

    Map map(&sync_random, &text_painter);
    EXPECT_EQ(map.GetWidth(), 0);
    EXPECT_EQ(map.GetHeight(), 0);
    EXPECT_EQ(map.GetDepth(), 0);

    EXPECT_EQ(map.GetSquares().size(), 0);
}

