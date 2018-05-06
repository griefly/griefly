#include "KvAbort.h"

#include <gtest/gtest.h>

TEST(KvAbortDeathTest, Basics)
{
    ASSERT_DEATH(
    {
        kv::Abort("Some error message");
    }, "Some error message");
}

TEST(KvAssertDeathTest, Basics)
{
    ASSERT_DEATH(
    {
        kv::Assert(false, "Some error message");
    }, "Some error message");

    // TODO: how to ASSERT_NO_DEATH?
    kv::Assert(true, "No error here");
}
