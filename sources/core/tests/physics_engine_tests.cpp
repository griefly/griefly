#include <gtest/gtest.h>

#include "objects/PhysicsEngine.h"

using namespace kv;

TEST(PhysicsEngine, ApplyForce)
{
    int progress = 0;
    Vector force(1, 1, 1);
    PhysicsEngine::ApplyForce(&force, &progress, {1, 0, -1});
    EXPECT_EQ(force.x, 2);
    EXPECT_EQ(force.y, 1);
    EXPECT_EQ(force.z, 0);
    EXPECT_EQ(progress, 0);

    PhysicsEngine::ApplyForce(&force, &progress, {98, 99, 0});
    EXPECT_EQ(force.x, 100);
    EXPECT_EQ(force.y, 100);
    EXPECT_EQ(force.z, 0);
    EXPECT_EQ(progress, 0);

    progress = 5000;
    PhysicsEngine::ApplyForce(&force, &progress, {1, 1, 1});
    EXPECT_EQ(force.x, 76);
    EXPECT_EQ(force.y, 76);
    EXPECT_EQ(force.z, 1);
    EXPECT_EQ(progress, 0);
}
