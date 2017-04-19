#include <gtest/gtest.h>

#include "core/Game.h"

TEST(Game, PlayersIds)
{
    Game game;

    for (int i = 0; i < 100; ++i)
    {
        EXPECT_EQ(game.GetNetId(i), 0);
        EXPECT_EQ(game.GetPlayerId(i), 0);
    }

    game.SetPlayerId(1, 9915);
    EXPECT_EQ(game.GetNetId(9915), 1);
    EXPECT_EQ(game.GetPlayerId(1), 9915);

    for (int i = 100; i < 200; ++i)
    {
        EXPECT_EQ(game.GetNetId(i), 0);
        EXPECT_EQ(game.GetPlayerId(i), 0);
    }

    game.SetPlayerId(543, 10000000);
    EXPECT_EQ(game.GetNetId(10000000), 543);
    EXPECT_EQ(game.GetPlayerId(543), 10000000);
}
