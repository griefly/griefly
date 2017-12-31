#include "KvAbort.h"

#include <gtest/gtest.h>

TEST(KvAbortDeathTest, Basics)
{
    ASSERT_DEATH(
    {
        kv::Abort("Some error message");
    }, "Some error message");
}
