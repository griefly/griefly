#include "PhysicsEngine.h"

#include "movable/Movable.h"

using namespace kv;

PhysicsEngine::PhysicsEngine()
{
    // Nothing
}

void PhysicsEngine::AfterWorldCreation()
{
    Object::AfterWorldCreation();
}

void PhysicsEngine::Add(IdPtr<Movable> movable)
{
    to_add_.push_back(movable);
}

Dir PhysicsEngine::ProcessForceTick(Vector* force, qint64* progress, int friction, int mass)
{
    // TODO: a-la Bresenham algo here
    Q_UNUSED(mass)
    const Dir retval = VDirToDir(*force);
    if (friction == 0)
    {
        return retval;
    }
    // TODO: proper helper
    if ((retval == Dir::NORTH) || (retval == Dir::SOUTH))
    {
        if (std::abs(force->y) < FORCE_UNIT)
        {
            return Dir::ALL;
        }
    }
    else
    {
        if (std::abs(force->x) < FORCE_UNIT)
        {
            return Dir::ALL;
        }
    }
    Vector temp = DirToVDir(retval);
    temp *= FORCE_UNIT;
    *force -= temp;
    return retval;
}

void PhysicsEngine::ApplyForce(Vector* force, qint64* progress, const Vector& addition)
{
    *force += addition;

    const qint64 x = std::max(Q_INT64_C(1), std::abs(force->x));
    const qint64 y = std::max(Q_INT64_C(1), std::abs(force->y));

    const qint64 ratio = std::max(x, y) / std::min(x, y);

    if (ratio < *progress)
    {
        *progress = *progress % ratio;
    }
}

void PhysicsEngine::ProcessPhysics()
{
    // TODO: better clearing algorithm #443
    Clear();

    for (auto movable = to_add_.begin(); movable != to_add_.end(); ++movable)
    {
        if (!(*movable).IsValid())
        {
            continue;
        }
        if (!IsNonZero((*movable)->force_))
        {
            continue;
        }

        auto to_add = std::find(under_force_.begin(), under_force_.end(), *movable);
        if (to_add == under_force_.end())
        {
            under_force_.push_back(*movable);
        }
    }
    if (to_add_.size())
    {
        std::sort(under_force_.begin(), under_force_.end(),
        [](IdPtr<Movable> item1, IdPtr<Movable> item2)
        {
            return item1.Id() < item2.Id();
        });
        to_add_.clear();
    }

    for (auto movable = under_force_.begin(); movable != under_force_.end(); ++movable)
    {
        if (   !(*movable)
            || !IsNonZero((*movable)->force_))
        {
            continue;
        }
        (*movable)->ProcessForce();
    }
}

void PhysicsEngine::Clear()
{
    std::vector<IdPtr<Movable>> to_remove;
    for (auto movable = under_force_.begin(); movable != under_force_.end(); ++movable)
    {
        if (   !(*movable)
            || !IsNonZero((*movable)->force_))
        {
            to_remove.push_back(*movable);
        }
    }
    for (auto it = to_remove.begin(); it != to_remove.end(); ++it)
    {
        under_force_.erase(std::find(under_force_.begin(), under_force_.end(), *it));
    }
}
