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

TEST(Vector, OperatorMultiply)
{
    const Vector vector(1, 2, 3);

    {
        const Vector result = vector * 4;
        EXPECT_EQ(result.x, 4);
        EXPECT_EQ(result.y, 8);
        EXPECT_EQ(result.z, 12);
    }
    {
        const Vector result2 = vector * -1;
        EXPECT_EQ(result2.x, -1);
        EXPECT_EQ(result2.y, -2);
        EXPECT_EQ(result2.z, -3);
    }
    {
        const Vector result3 = 4 * vector;
        EXPECT_EQ(result3.x, 4);
        EXPECT_EQ(result3.y, 8);
        EXPECT_EQ(result3.z, 12);
    }
    {
        const Vector result4 = -1 * vector;
        EXPECT_EQ(result4.x, -1);
        EXPECT_EQ(result4.y, -2);
        EXPECT_EQ(result4.z, -3);
    }
}

TEST(Vector, OperatorMultiplyEq)
{
    Vector vector(1, 2, 3);

    vector *= 4;
    EXPECT_EQ(vector.x, 4);
    EXPECT_EQ(vector.y, 8);
    EXPECT_EQ(vector.z, 12);

    vector *= -1;
    EXPECT_EQ(vector.x, -4);
    EXPECT_EQ(vector.y, -8);
    EXPECT_EQ(vector.z, -12);
}

TEST(Vector, OperatorMinus)
{
    const Vector vector(1, 1, 1);
    const Vector vector2(-1, 2, 10);

    {
        const Vector result = vector - vector2;
        EXPECT_EQ(result.x, 2);
        EXPECT_EQ(result.y, -1);
        EXPECT_EQ(result.z, -9);
    }
    {
        const Vector result = vector2 - vector;
        EXPECT_EQ(result.x, -2);
        EXPECT_EQ(result.y, 1);
        EXPECT_EQ(result.z, 9);
    }
}

TEST(Vector, OperatorMinusEq)
{
    Vector vector(1, 1, 1);
    const Vector vector2(-1, 2, 10);

    vector -= vector2;
    EXPECT_EQ(vector.x, 2);
    EXPECT_EQ(vector.y, -1);
    EXPECT_EQ(vector.z, -9);

    vector -= vector2;
    EXPECT_EQ(vector.x, 3);
    EXPECT_EQ(vector.y, -3);
    EXPECT_EQ(vector.z, -19);
}
