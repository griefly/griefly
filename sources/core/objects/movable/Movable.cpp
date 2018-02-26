#include "Movable.h"

#include "objects/mobs/Mob.h"

#include "objects/PhysicsEngine.h"

#include "objects/turfs/Turf.h"

#include "objects/GlobalObjectsHolder.h"

using namespace kv;

Movable::Movable()
{
    last_move_ = 0;
    tick_speed_ = 1;
    direction_ = Dir::SOUTH;
    anchored_ = false;
    force_.x = 0;
    force_.y = 0;
    force_.z = 0;
    force_progress_ = 0;
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
    if (!IsNonZero(force_))
    {
        return;
    }

    const Dir step = PhysicsEngine::ProcessForceTick(
        &force_, &force_progress_, friction::CombinedFriction(GetTurf()), 1);
    if (step == Dir::ALL)
    {
        return;
    }

    TryMove(step);
}

void Movable::ApplyForce(Vector force)
{
    if (IsZero(force))
    {
        return;
    }
    if (IsZero(force_))
    {
        GetGame().GetGlobals()->physics_engine->Add(GetId());
    }

    PhysicsEngine::ApplyForce(&force_, &force_progress_, force);
}

bool Movable::CheckMoveTime()
{
    if ((GetGameTick() - last_move_) < tick_speed_)
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

    last_move_ = GetGameTick();
    return true;
}

void Movable::Represent(GrowingFrame* frame, IdPtr<kv::Mob> mob)
{
    FrameData::Entity entity;
    entity.id = GetId();
    entity.click_id = GetId();

    const Position position = GetPosition();
    entity.pos_x = position.x;
    entity.pos_y = position.y;

    entity.vlevel = v_level;
    entity.view = GetView()->GetRawData();
    entity.dir = GetDir();
    frame->Append(entity);
}

void Movable::Bump(IdPtr<Movable> item)
{
    if (IdPtr<Mob> mob = item)
    {
        ApplyForce(DirToVDir(mob->GetDir()) * PhysicsEngine::FORCE_UNIT);
    }
}

void Movable::BumpByGas(Dir dir, bool inside)
{
    ApplyForce(DirToVDir(dir) * PhysicsEngine::FORCE_UNIT);
}
