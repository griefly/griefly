#include <gtest/gtest.h>

#include "objects/Object.h"
#include "Hashes.h"

using namespace kv;

TEST(HashesTest, IntegerNumber)
{
    EXPECT_EQ(Hash(static_cast<unsigned int>(0)), 0);
    EXPECT_EQ(Hash(1), 1);
    EXPECT_EQ(Hash(42), 42);

    EXPECT_NE(Hash(54), 53);

    unsigned int test = std::numeric_limits<unsigned int>::max();
    EXPECT_EQ(Hash(test), test);
}

TEST(HashesTest, IdPtrOn)
{
    IdPtr<kv::Object> ptr = 0;
    EXPECT_EQ(Hash(ptr), 0);

    ptr = 131;
    EXPECT_EQ(Hash(ptr), 131);

    ptr = 1000000;
    EXPECT_EQ(Hash(ptr), 1000000);
}

TEST(HashesTest, String)
{
    QString s1("hello world");
    EXPECT_EQ(Hash(s1), 1045060184);

    s1[4] = '\0';
    EXPECT_EQ(Hash(s1), 2292011938);
}

TEST(HashesTest, bitset)
{
    std::bitset<4> bits;
    for (int value = 0; value < 4; value++)
    {
        bits = std::bitset<4>(value);
        EXPECT_EQ(Hash(bits), value);
    }
}
