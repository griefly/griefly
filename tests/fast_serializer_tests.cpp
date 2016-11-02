#include <gtest/gtest.h>

#include "FastSerializer.h"

TEST(FastSerializer, Constructor)
{
    FastSerializer serializer;
    ASSERT_EQ(serializer.GetIndex(), 0);
    ASSERT_TRUE(serializer.GetData());
}
