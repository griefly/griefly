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

Dir PhysicsEngine::ProcessForceTick(
    Vector* force, Dir main, Dir secondary, qint32* error,
    qint32 error_per_main, int friction, int mass)
{
    // TODO: use mass
    Q_UNUSED(mass)

    Dir retval = main;
    Dir other = secondary;
    if (*error >= ERROR_SCALE / 2)
    {
        std::swap(retval, other);
        *error -= ERROR_SCALE;
    }
    else
    {
        *error += error_per_main;
    }

    if (ProjectionToDir(*force, retval) < FORCE_UNIT)
    {
        if (ProjectionToDir(*force, other) >= FORCE_UNIT)
        {
            std::swap(retval, other);
        }
        else
        {
            *error = 0;
            return Dir::ALL;
        }
    }

    if (friction == 0)
    {
        return retval;
    }

    Vector temp = DirToVDir(retval);
    temp *= FORCE_UNIT;
    *force -= temp;
    return retval;
}

void PhysicsEngine::ApplyForce(
    Vector* force, Dir* main, Dir* secondary, qint32* error,
    qint32* error_per_main, const Vector& addition)
{
    *force += addition;

    const Dir old_secondary = *secondary;

    *main = (force->x > 0) ? Dir::EAST : Dir::WEST;
    *secondary = (force->y > 0) ? Dir::SOUTH : Dir::NORTH;

    if (std::abs(force->x) < std::abs(force->y))
    {
        std::swap(*main, *secondary);
    }

    if (old_secondary != *secondary)
    {
        *error = 0;
    }

    const int x = std::abs(force->x);
    const int y = std::abs(force->y);
    *error_per_main = (std::min(x, y) * ERROR_SCALE) / std::max(1, std::max(x, y));
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
