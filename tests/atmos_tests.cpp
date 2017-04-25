#include <gtest/gtest.h>

#include "core/atmos/Atmos.h"
#include "interfaces_mocks.h"

#include "representation/Text.h"

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

TEST_F(AtmosDeathTest, ResizeWidth)
{
    MockIMap map;
    TextPainter painter;
    Atmosphere atmos(&map, &painter);

    ASSERT_DEATH(
    {
        atmos.Resize(17, 32, 1);
    }, "Width is not multiplier of 32!");
}

TEST_F(AtmosDeathTest, ResizeHeight)
{
    MockIMap map;
    TextPainter painter;
    Atmosphere atmos(&map, &painter);

    ASSERT_DEATH(
    {
        atmos.Resize(32, 17, 1);
    }, "Height is not multiplier of 32!");
}

TEST(AtmosTest, Resize)
{
    MockIMap map;
    TextPainter painter;

    Atmosphere atmos(&map, &painter);
    atmos.Resize(32, 32, 1);
}
