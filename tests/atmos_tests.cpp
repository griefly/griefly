#include <gtest/gtest.h>

#include "core/atmos/Atmos.h"
#include "interfaces_mocks.h"

#include "representation/Text.h"

using ::testing::ReturnRef;
using ::testing::Return;

class AtmosTest : public ::testing::Test
{
protected:
    AtmosTest()
    {
        // Nothing
    }

    MockIMap map;
    Atmosphere atmos;
};

using AtmosDeathTest = AtmosTest;

TEST_F(AtmosDeathTest, ProcessWithoutLoadGrid)
{
    ASSERT_DEATH(
    {
        atmos.Process(0);
    }, "Grid is not loaded");
}

TEST_F(AtmosDeathTest, ProcessMoveWithoutLoadGrid)
{
    ASSERT_DEATH(
    {
        atmos.ProcessMove(0);
    }, "Grid is not loaded");
}

TEST_F(AtmosDeathTest, SetFlagWithoutLoadGrid)
{
    ASSERT_DEATH(
    {
        atmos.SetFlags(0, 0, 0, atmos::CLEAR);
    }, "Grid is not loaded");
}

TEST_F(AtmosDeathTest, LoadGridWidthFail)
{
    ASSERT_DEATH(
    {
        EXPECT_CALL(map, GetWidth())
            .WillOnce(Return(17));
        EXPECT_CALL(map, GetHeight())
            .WillOnce(Return(32));
        EXPECT_CALL(map, GetDepth())
            .WillOnce(Return(1));
        atmos.LoadGrid(&map);
    }, "Width is not multiplier of 32!");
}

TEST_F(AtmosDeathTest, LoadGridHeightFail)
{
    ASSERT_DEATH(
    {
        EXPECT_CALL(map, GetWidth())
            .WillOnce(Return(32));
        EXPECT_CALL(map, GetHeight())
            .WillOnce(Return(17));
        EXPECT_CALL(map, GetDepth())
            .WillOnce(Return(1));
        atmos.LoadGrid(&map);
    }, "Height is not multiplier of 32!");
}
