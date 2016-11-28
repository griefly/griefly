#include <gtest/gtest.h>

#include "interfaces_mocks.h"

#include "core/ObjectFactory.h"

TEST(ObjectFactory, Constructor)
{
    MockIGame game;
    ObjectFactory factory(&game);
    EXPECT_EQ(factory.Hash(), 0);
    for (auto info : factory.GetIdTable())
    {
        EXPECT_EQ(info.object, nullptr);
    }
}
