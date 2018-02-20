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

    PhysicsEngine::ApplyForce(&force, &progress, {8, 9, 0});
    EXPECT_EQ(force.x, 10);
    EXPECT_EQ(force.y, 10);
    EXPECT_EQ(force.z, 0);
    EXPECT_EQ(progress, 0);

    progress = 50 * PhysicsEngine::PROGRESS_MULTIPLIER;
    PhysicsEngine::ApplyForce(&force, &progress, {1, 1, 1});
    EXPECT_EQ(force.x, 8);
    EXPECT_EQ(force.y, 8);
    EXPECT_EQ(force.z, 1);
    EXPECT_EQ(progress, 0);
}
