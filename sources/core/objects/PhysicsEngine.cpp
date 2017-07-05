#include "PhysicsEngine.h"

#include "movable/Movable.h"

using namespace kv;

PhysicsEngine& PhysicsEngine::Get()
{
    static PhysicsEngine* engine = new PhysicsEngine;
    return *engine;
}

void PhysicsEngine::Add(IdPtr<Movable> movable)
{
    to_add_.push_back(movable);
}

unsigned int PhysicsEngine::Hash()
{
    unsigned int hash = 0;
    int i = 1;
    for (auto movable = under_force_.begin(); movable != under_force_.end(); ++movable)
    {
        hash += movable->Id() * i;
        ++i;
    }
    return hash;
}

void PhysicsEngine::Process()
{
    const int CLEAR_TICK = 10;
    if (true/* TODO: when PhysicsEngine will be derived from kv::Object GetGameTick() % CLEAR_TICK == 1*/)
    {
        Clear();
    }

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
