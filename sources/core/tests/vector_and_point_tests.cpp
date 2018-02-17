#include <gtest/gtest.h>

#include "KvGlobals.h"

using namespace kv;

TEST(Vector, Constructor)
{
    const Vector vector;
    EXPECT_EQ(vector.x, 0);
    EXPECT_EQ(vector.y, 0);
    EXPECT_EQ(vector.z, 0);

    const Vector vector2(-1, 15, 20);
    EXPECT_EQ(vector2.x, -1);
    EXPECT_EQ(vector2.y, 15);
    EXPECT_EQ(vector2.z, 20);
}

TEST(Vector, OperatorPlus)
{
    const Vector vector(1, 1, 1);
    const Vector vector2(-1, 2, 10);
    const Vector sum = vector + vector2;
    EXPECT_EQ(sum.x, 0);
    EXPECT_EQ(sum.y, 3);
    EXPECT_EQ(sum.z, 11);
}

TEST(Vector, OperatorPlusEq)
{
    Vector vector(1, 1, 1);
    const Vector vector2(-1, 2, 10);
    vector += vector2;
    EXPECT_EQ(vector.x, 0);
    EXPECT_EQ(vector.y, 3);
    EXPECT_EQ(vector.z, 11);
}
