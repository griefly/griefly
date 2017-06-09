#include "Movable.h"

#include "../Helpers.h"
#include "Mob.h"

#include "../Game.h"

#include <cassert>

using namespace kv;

Movable::Movable()
{
    last_move_ = 0;
    tick_speed_ = 1;
    direction_ = Dir::DOWN;
    anchored_ = false;
    force_.x = 0;
    force_.y = 0;
    force_.z = 0;
}

bool Movable::TryMove(Dir direct)
{
    if (!CheckMoveTime())
    {
        return false;
    }
    if (anchored_)
    {
        return false;
    }
    if (!Rotate(direct))
    {
        return false;
    }
    if (!CheckPassable())
    {
        return false;
    }
    return MainMove();
}

void Movable::ProcessForce()
{
    Dir step = VDirToDir(force_);

    TryMove(step);

    if (!IsNonZero(force_))
    {
        return;
    }

    Vector vstep = DirToVDir(step);
    force_.x -= (vstep.x * friction::CombinedFriction(GetTurf())) / friction::BASE_FRICTION;
    force_.y -= (vstep.y * friction::CombinedFriction(GetTurf())) / friction::BASE_FRICTION;
    force_.z -= (vstep.z * friction::CombinedFriction(GetTurf())) / friction::BASE_FRICTION;
}

void Movable::ApplyForce(Vector force)
{
    if (!IsNonZero(force))
    {
        return;
    }
    if (!IsNonZero(force_))
    {
        ForceManager::Get().Add(GetId());
    }

    force_.x += force.x;
    force_.y += force.y;
    force_.z += force.z;
}

void Movable::LoadInForceManager()
{
    if (IsNonZero(force_))
    {
        ForceManager::Get().Add(GetId());
    }
}

bool Movable::CheckMoveTime()
{
    if ((static_cast<int>(MAIN_TICK) - last_move_) < tick_speed_)
    {
        return false;
    }
    return true;
}

bool Movable::CheckPassable()
{
    PassableLevel loc = GetPassable(GetDir());
    if (loc != passable::FULL)
    {
        SetPassable(GetDir(), passable::FULL);
    }
    auto owner = GetOwner();
    if (!CanPass(owner->GetPassable(GetDir()), passable_level))
    {
        owner->Bump(GetId());
        force_.x = 0;
        force_.y = 0;
        force_.z = 0;
        if (loc != passable::FULL)
        {
            SetPassable(GetDir(), loc);
        }
        return false;
    }
    if (loc != passable::FULL)
    {
        SetPassable(GetDir(), loc);
    }

    auto neighbour = owner->GetNeighbour(GetDir());
    if (   !CanPass(neighbour->GetPassable(Dir::ALL), passable_level)
        || !CanPass(neighbour->GetPassable(RevertDir(GetDir())), passable_level))
    {
        neighbour->Bump(GetId());
        force_.x = 0;
        force_.y = 0;
        force_.z = 0;
        return false;
    }
    
    return true;
}

bool Movable::Rotate(Dir dir)
{
    direction_ = dir;
    return true;
}

bool Movable::MainMove()
{
    auto new_owner = GetOwner()->GetNeighbour(GetDir());
    if (new_owner == GetOwner())
    {
        return false;
    }

    GetOwner()->RemoveObject(GetId());
    new_owner->AddObject(GetId());

    last_move_ = static_cast<int>(MAIN_TICK);
    return true;
}

void Movable::Represent()
{
    Representation::Entity ent;
    ent.id = GetId();
    ent.click_id = GetId();

    const Position position = GetPosition();
    ent.pos_x = position.x;
    ent.pos_y = position.y;

    ent.vlevel = v_level;
    ent.view = *GetView();
    ent.dir = GetDir();
    GetRepresentation().AddToNewFrame(ent);
}

void Movable::Bump(IdPtr<Movable> item)
{
    if (IdPtr<Mob> mob = item)
    {
        ApplyForce(DirToVDir(mob->GetDir()));
    }
}

void Movable::BumpByGas(Dir dir, bool inside)
{
    ApplyForce(DirToVDir(dir));
}

ForceManager& ForceManager::Get()
{
    static ForceManager* fm = new ForceManager;
    return *fm;
}

void ForceManager::Add(IdPtr<Movable> movable)
{
    to_add_.push_back(movable);
}

unsigned int ForceManager::Hash()
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

void ForceManager::Process()
{
    const int CLEAR_TICK = 10;
    if (MAIN_TICK % CLEAR_TICK == 1)
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

void ForceManager::Clear()
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
