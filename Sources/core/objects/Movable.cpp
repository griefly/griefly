#include "Movable.h"

#include "../helpers.h"
#include "Mob.h"
#include "../MobPosition.h"

#include "Creator.h"
#include "../Manager.h"

#include <assert.h>

IMovable::IMovable(size_t id) : IOnMapObject(id)
{
    lastMove = 0;
    tickSpeed = 1;
    pixSpeed = 1;
    dMove = D_DOWN;
    anchored = false;
    force_.x = 0;
    force_.y = 0;
    force_.z = 0;
}

bool IMovable::checkMove(Dir direct)
{
    if (!checkMoveTime()) 
        return false;
    if (anchored)
        return false;
    if (!Rotate(direct))
        return false;
    if (!checkPassable())
    {
        return false;
    }
    return mainMove();    
};

void IMovable::ProcessForce()
{

    Dir step = VDirToDir(force_);

    checkMove(step);

    if (!NonZero(force_))
        return;

    // qDebug() << "Process force: " << GetId();
    // qDebug() << force_.x;
    // qDebug() << force_.y;

    VDir vstep = DirToVDir[step];
    force_.x -= (vstep.x * Friction::CombinedFriction(GetTurf())) / Friction::BASE_FRICTION;
    force_.y -= (vstep.y * Friction::CombinedFriction(GetTurf())) / Friction::BASE_FRICTION;
    force_.z -= (vstep.z * Friction::CombinedFriction(GetTurf())) / Friction::BASE_FRICTION;
}

void IMovable::ApplyForce(VDir force)
{
    if (!NonZero(force))
        return;
    if (!NonZero(force_))
        ForceManager::Get().Add(GetId());

    force_.x += force.x;
    force_.y += force.y;
    force_.z += force.z;
}

void IMovable::LoadInForceManager()
{
    if (NonZero(force_))
        ForceManager::Get().Add(GetId());
}

bool IMovable::checkMoveTime()
{
    if((static_cast<int>(MAIN_TICK) - lastMove) < tickSpeed) 
        return false;
    return true;
};

// TODO: ТОРНАДО
bool IMovable::checkPassable()
{
    PassableLevel loc = GetPassable(dMove);
    if (loc != Passable::FULL)
    {
        SetPassable(dMove, Passable::FULL);
    }
    if (!CanPass(owner->GetPassable(dMove), passable_level))
    {
        owner->Bump(GetId());
        force_.x = 0;
        force_.y = 0;
        force_.z = 0;
        if (loc != Passable::FULL)
        {
            SetPassable(dMove, loc);
        }
        return false;
    }
    if (loc != Passable::FULL)
    {
        SetPassable(dMove, loc);
    }

    auto neighbour = owner->GetNeighbour(dMove);
    if (   !CanPass(neighbour->GetPassable(D_ALL), passable_level)
        || !CanPass(neighbour->GetPassable(helpers::revert_dir(dMove)), passable_level))
    {
        neighbour->Bump(GetId());
        force_.x = 0;
        force_.y = 0;
        force_.z = 0;
        return false;
    }
    
    return true;
}

bool IMovable::Rotate(Dir dir)
{
    dMove = dir;
    return true;
}

bool IMovable::mainMove()
{
    auto new_owner = owner->GetNeighbour(dMove);
    if (new_owner == owner)
        return false;

    owner->RemoveItem(GetId());
    new_owner->AddItem(GetId());

    if (   GetMob().ret_id() == GetId()
        || Contains(GetMob()))
    {
        GetManager().UpdateVisible();
    }

    lastMove = static_cast<int>(MAIN_TICK);
    return true;
}

void IMovable::Represent()
{
    Representation::Entity ent;
    ent.id = GetId();
    ent.pos_x = GetX();
    ent.pos_y = GetY();
    ent.vlevel = v_level;
    ent.view = *GetView();
    ent.dir = GetDir();
    GetRepresentation().AddToNewFrame(ent);
}

void IMovable::Bump(id_ptr_on<IMovable> item)
{
    id_ptr_on<IMob> m;
    m = item;
    if (m)
        ApplyForce(DirToVDir[m->dMove]);
}

void IMovable::BumpByGas(Dir dir, bool inside)
{
    //checkMove(dir);
    ApplyForce(DirToVDir[dir]);
}

ForceManager& ForceManager::Get()
{
    static ForceManager* fm = new ForceManager;
    return *fm;
}

void ForceManager::Add(id_ptr_on<IMovable> movable)
{
    to_add_.push_back(movable);
}

unsigned int ForceManager::Hash()
{
    unsigned int hash = 0;
    int i = 1;
    for (auto movable = under_force_.begin(); movable != under_force_.end(); ++movable)
    {
        hash += movable->ret_id() * i;
        ++i;
    }
    return hash;
}

void ForceManager::Process()
{
    Timer timer, remove_timer;
    timer.Start();
    for (auto movable = to_add_.begin(); movable != to_add_.end(); ++movable)
    {
        if (!(*movable).valid())
            continue;
        if (!NonZero((*movable)->force_))
            continue;

        auto to_add = std::find(under_force_.begin(), under_force_.end(), *movable);
        if (to_add == under_force_.end())
        {
            under_force_.push_back(*movable);
        }
    }
    if (to_add_.size())
    {
        std::sort(under_force_.begin(), under_force_.end(),
        [](id_ptr_on<IMovable> item1, id_ptr_on<IMovable> item2)
        {
            return item1.ret_id() < item2.ret_id();
        });
        to_add_.clear();
    }

  //  SYSTEM_STREAM << "Remove size: " << to_remove.size() << " Force size: " << under_force_.size() << std::endl;
    remove_timer.Start();
    for (auto movable = under_force_.begin(); movable != under_force_.end(); ++movable)
    {
        if (   !(*movable)
            || !NonZero((*movable)->force_))
        {
            continue;
        }
        (*movable)->ProcessForce();
    }
  //  SYSTEM_STREAM << "Remove take: " << (remove_timer.Get() * 1.0 / timer.Get()) * 100.0 << "%" << std::endl;
  //  SYSTEM_STREAM << "Remove size: " << to_remove.size() << " Force size: " << under_force_.size() << std::endl;
    //SYSTEM_STREAM << "Force take: " << timer.Get() / 1000.0 << std::endl;
}

void ForceManager::Clear()
{
    std::vector<id_ptr_on<IMovable>> to_remove;
    for (auto movable = under_force_.begin(); movable != under_force_.end(); ++movable)
    {
        if (   !(*movable)
            || !NonZero((*movable)->force_))
        {
            to_remove.push_back(*movable);
        }
    }
    for (auto it = to_remove.begin(); it != to_remove.end(); ++it)
        under_force_.erase(std::find(under_force_.begin(), under_force_.end(), *it));
}
