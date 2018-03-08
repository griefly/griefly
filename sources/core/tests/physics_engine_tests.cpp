#include <gtest/gtest.h>

#include "objects/PhysicsEngine.h"

using namespace kv;

TEST(PhysicsEngine, ApplyForce)
{
    Vector force(1, 1, 1);
    Dir main = Dir::EAST;
    Dir secondary = Dir::SOUTH;
    qint32 error = 1;
    qint32 error_per_main = 1 * ERROR_SCALE;
    PhysicsEngine::ApplyForce(
        &force, &main, &secondary, &error, &error_per_main, {1, 0, -1});
    EXPECT_EQ(force.x, 2);
    EXPECT_EQ(force.y, 1);
    EXPECT_EQ(force.z, 0);
    EXPECT_EQ(main, Dir::EAST);
    EXPECT_EQ(secondary, Dir::SOUTH);
    EXPECT_EQ(error, 1);
    EXPECT_EQ(error_per_main, ERROR_SCALE / 2);

    PhysicsEngine::ApplyForce(
        &force, &main, &secondary, &error, &error_per_main, {-3, 99, 0});
    EXPECT_EQ(force.x, -1);
    EXPECT_EQ(force.y, 100);
    EXPECT_EQ(force.z, 0);
    EXPECT_EQ(main, Dir::SOUTH);
    EXPECT_EQ(secondary, Dir::WEST);
    EXPECT_EQ(error, 0);
    EXPECT_EQ(error_per_main, ERROR_SCALE / 100);

    error = 42;
    PhysicsEngine::ApplyForce(
        &force, &main, &secondary, &error, &error_per_main, {0, -200, 0});
    EXPECT_EQ(force.x, -1);
    EXPECT_EQ(force.y, -100);
    EXPECT_EQ(force.z, 0);
    EXPECT_EQ(main, Dir::NORTH);
    EXPECT_EQ(secondary, Dir::WEST);
    EXPECT_EQ(error, 42);
    EXPECT_EQ(error_per_main, ERROR_SCALE / 100);
}

TEST(PhysicsEngine, ApplyForceZero)
{
    Vector force(0, 0, 0);
    Dir main = Dir::SOUTH;
    Dir secondary = Dir::SOUTH;
    qint32 error = 0;
    qint32 error_per_main = 0;
    PhysicsEngine::ApplyForce(
        &force, &main, &secondary, &error, &error_per_main, {0, 0, 0});
    EXPECT_EQ(force.x, 0);
    EXPECT_EQ(force.y, 0);
    EXPECT_EQ(force.z, 0);
    EXPECT_EQ(main, Dir::WEST);
    EXPECT_EQ(secondary, Dir::NORTH);
    EXPECT_EQ(error, 0);
    EXPECT_EQ(error_per_main, 0);

    error = 3;
    PhysicsEngine::ApplyForce(
        &force, &main, &secondary, &error, &error_per_main, {0, 0, 0});
    EXPECT_EQ(force.x, 0);
    EXPECT_EQ(force.y, 0);
    EXPECT_EQ(force.z, 0);
    EXPECT_EQ(main, Dir::WEST);
    EXPECT_EQ(secondary, Dir::NORTH);
    EXPECT_EQ(error, 3);
    EXPECT_EQ(error_per_main, 0);
}

TEST(PhysicsEngine, ProcessForceTick)
{
    Vector force(FORCE_UNIT, FORCE_UNIT * 2, 0);
    Dir main = Dir::SOUTH;
    Dir secondary = Dir::EAST;
    qint32 error = 0;
    qint32 error_per_main = ERROR_SCALE / 2;

    {
        const Dir retval = PhysicsEngine::ProcessForceTick(
            &force, main, secondary, &error, error_per_main, 1, 1);
        EXPECT_EQ(retval, Dir::SOUTH);
    }
    EXPECT_EQ(force.x, FORCE_UNIT);
    EXPECT_EQ(force.y, FORCE_UNIT);
    EXPECT_EQ(force.z, 0);
    EXPECT_EQ(error, ERROR_SCALE / 2);

    {
        const Dir retval = PhysicsEngine::ProcessForceTick(
            &force, main, secondary, &error, error_per_main, 1, 1);
        EXPECT_EQ(retval, Dir::EAST);
    }
    EXPECT_EQ(force.x, 0);
    EXPECT_EQ(force.y, FORCE_UNIT);
    EXPECT_EQ(force.z, 0);
    EXPECT_EQ(error, -1 * (ERROR_SCALE / 2));

    {
        const Dir retval = PhysicsEngine::ProcessForceTick(
            &force, main, secondary, &error, error_per_main, 1, 1);
        EXPECT_EQ(retval, Dir::SOUTH);
    }
    EXPECT_EQ(force.x, 0);
    EXPECT_EQ(force.y, 0);
    EXPECT_EQ(force.z, 0);
    EXPECT_EQ(error, 0);

    {
        const Dir retval = PhysicsEngine::ProcessForceTick(
            &force, main, secondary, &error, error_per_main, 1, 1);
        EXPECT_EQ(retval, Dir::ALL);
    }
    EXPECT_EQ(force.x, 0);
    EXPECT_EQ(force.y, 0);
    EXPECT_EQ(force.z, 0);
    EXPECT_EQ(error, 0);

    force.x = FORCE_UNIT;
    {
        const Dir retval = PhysicsEngine::ProcessForceTick(
            &force, main, secondary, &error, error_per_main, 1, 1);
        EXPECT_EQ(retval, Dir::EAST);
    }
    EXPECT_EQ(force.x, 0);
    EXPECT_EQ(force.y, 0);
    EXPECT_EQ(force.z, 0);
    EXPECT_EQ(error, 0);
}

TEST(PhysicsEngine, ProcessForceTickZeroFriction)
{
    Vector force(-2 * FORCE_UNIT, -1 * FORCE_UNIT, 0);
    Dir main = Dir::WEST;
    Dir secondary = Dir::NORTH;
    qint32 error = 0;
    qint32 error_per_main = ERROR_SCALE / 2;

    {
        const Dir retval = PhysicsEngine::ProcessForceTick(
            &force, main, secondary, &error, error_per_main, 0, 1);
        EXPECT_EQ(retval, Dir::WEST);
    }
    EXPECT_EQ(force.x, -2 * FORCE_UNIT);
    EXPECT_EQ(force.y, -1 * FORCE_UNIT);
    EXPECT_EQ(force.z, 0);
    EXPECT_EQ(error, ERROR_SCALE / 2);

    {
        const Dir retval = PhysicsEngine::ProcessForceTick(
            &force, main, secondary, &error, error_per_main, 0, 1);
        EXPECT_EQ(retval, Dir::NORTH);
    }
    EXPECT_EQ(force.x, -2 * FORCE_UNIT);
    EXPECT_EQ(force.y, -1 * FORCE_UNIT);
    EXPECT_EQ(force.z, 0);
    EXPECT_EQ(error, -1 * (ERROR_SCALE / 2));

    {
        const Dir retval = PhysicsEngine::ProcessForceTick(
            &force, main, secondary, &error, error_per_main, 0, 1);
        EXPECT_EQ(retval, Dir::WEST);
    }
    EXPECT_EQ(force.x, -2 * FORCE_UNIT);
    EXPECT_EQ(force.y, -1 * FORCE_UNIT);
    EXPECT_EQ(force.z, 0);
    EXPECT_EQ(error, 0);
}
