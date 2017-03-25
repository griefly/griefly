#include "KvAbort.h"

#include <gtest/gtest.h>

TEST(KvAbortDeathTest, Basics)
{
    ASSERT_DEATH(
    {
        KvAbort("Some error message");
    }, "Some error message");
}
