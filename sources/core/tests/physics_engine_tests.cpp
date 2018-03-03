#include <gtest/gtest.h>

#include "objects/PhysicsEngine.h"

using namespace kv;

TEST(PhysicsEngine, ApplyForce)
{
    // TODO: fix tests
    /*qint64 progress = 0;
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
    EXPECT_EQ(progress, 0);*/
}

TEST(PhysicsEngine, ApplyForceZero)
{
    /*qint64 progress = 0;
    Vector force(0, 0, 0);
    PhysicsEngine::ApplyForce(&force, &progress, {0, 0, 0});
    EXPECT_EQ(force.x, 0);
    EXPECT_EQ(force.y, 0);
    EXPECT_EQ(force.z, 0);
    EXPECT_EQ(progress, 0);

    progress = 1000;
    PhysicsEngine::ApplyForce(&force, &progress, {0, 0, 0});
    EXPECT_EQ(force.x, 0);
    EXPECT_EQ(force.y, 0);
    EXPECT_EQ(force.z, 0);
    EXPECT_EQ(progress, 0);

    progress = 0;
    PhysicsEngine::ApplyForce(&force, &progress, {1, 1, 1});
    EXPECT_EQ(force.x, 1);
    EXPECT_EQ(force.y, 1);
    EXPECT_EQ(force.z, 1);
    EXPECT_EQ(progress, 0);*/
}
