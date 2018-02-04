#include <gtest/gtest.h>

#include "objects/ObjectProcessor.h"

using namespace kv;

TEST(ObjectProcessor, Constructor)
{
    ObjectProcessor processor;
    EXPECT_EQ(processor.HashMembers(), 0);
}
