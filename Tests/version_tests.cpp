#include <gtest/gtest.h>

#include "Version.h"

TEST(Version, Basics)
{
    std::string v = get_game_version();
    ASSERT_FALSE(v.empty());
    ASSERT_TRUE(v[0] == 'v');
}
