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
    force_ = {0, 0, 0};
    main_force_direction_ = Dir::SOUTH;
    secondary_force_direction_ = Dir::SOUTH;
    force_error_ = 0;
    force_error_per_main_ = 0;
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

    const std::pair<Dir, Vector> step = PhysicsEngine::ProcessForceTick(
        force_,
        main_force_direction_,
        secondary_force_direction_,
        &force_error_,
        force_error_per_main_,
        1);
    if (step.first == Dir::ALL)
    {
        return;
    }

    if (!TryMove(step.first))
    {
        force_error_ = 0;
        force_ = {0, 0, 0};
        return;
    }

    if (friction::CombinedFriction(GetTurf()))
    {
        force_ -= step.second;
    }
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

    PhysicsEngine::ApplyForce(
        &force_,
        &main_force_direction_,
        &secondary_force_direction_,
        &force_error_,
        &force_error_per_main_,
        force);
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
    if (!CanPass(owner->GetPassable(GetDir()), GetPassableLevel()))
    {
        owner->Bump(force_, GetId());
        force_ = {0, 0, 0};
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
    if (   !CanPass(neighbour->GetPassable(Dir::ALL), GetPassableLevel())
        || !CanPass(neighbour->GetPassable(RevertDir(GetDir())), GetPassableLevel()))
    {
        neighbour->Bump(force_, GetId());
        force_ = {0, 0, 0};
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

    entity.vlevel = GetVisibleLevel();
    entity.view = GetView().GetRawData();
    entity.dir = GetDir();
    frame->Append(entity);
}

void Movable::Bump(const Vector& force, IdPtr<Movable> item)
{
    if (IdPtr<Mob> mob = item)
    {
        ApplyForce(force);
    }
}

void Movable::BumpByGas(const Vector& force, bool inside)
{
    ApplyForce(force);
}
