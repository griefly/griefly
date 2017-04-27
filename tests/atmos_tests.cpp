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
        : atmos(&map, &painter)
    {
        // Nothing
    }

    MockIMap map;
    TextPainter painter;
    Atmosphere atmos;
};

using AtmosDeathTest = AtmosTest;

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
        atmos.LoadGrid();
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
        atmos.LoadGrid();
    }, "Height is not multiplier of 32!");
}
