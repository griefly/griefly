#include <gtest/gtest.h>

#include "Version.h"

TEST(Version, Basics)
{
    const std::string v = kv::GetGameVersion();
    ASSERT_FALSE(v.empty());
    ASSERT_TRUE(v[0] == 'v');
}
