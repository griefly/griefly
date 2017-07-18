#include <gtest/gtest.h>

#include "core/objects/Object.h"
#include "core/Hashes.h"

using namespace kv;

TEST(HashesTest, IntegerNumber)
{
    ASSERT_EQ(Hash(static_cast<unsigned int>(0)), 0);
    ASSERT_EQ(Hash(1), 1);
    ASSERT_EQ(Hash(42), 42);

    ASSERT_NE(Hash(54), 53);

    unsigned int test = std::numeric_limits<unsigned int>::max();
    ASSERT_EQ(Hash(test), test);
}

TEST(HashesTest, IdPtrOn)
{
    IdPtr<kv::Object> ptr = 0;
    ASSERT_EQ(Hash(ptr), 0);

    ptr = 131;
    ASSERT_EQ(Hash(ptr), 131);

    ptr = 1000000;
    ASSERT_EQ(Hash(ptr), 1000000);
}

TEST(HashesTest, VectorIdPtrOn)
{
    std::vector<IdPtr<kv::Object>> v;
    v.push_back(0);
    v.push_back(1);
    v.push_back(2);
    ASSERT_EQ(Hash(v), 8);

    v[0] = 4294967295;
    ASSERT_EQ(Hash(v), 7);
}

TEST(HashesTest, String)
{
    QString s1("hello world");
    ASSERT_EQ(Hash(s1), 1045060184);

    s1[4] = '\0';
    ASSERT_EQ(Hash(s1), 2292011938);
}



