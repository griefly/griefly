#include "IMovable.h"

#include "EffectSystem.h"
#include "MoveEffect.h"
#include "helpers.h"
#include "MobInt.h"
#include "mob_position.h"

#include <assert.h>

IMovable::IMovable()
{
    lastMove = 0;
    tickSpeed = 1;
    pixSpeed = 1;
    dMove = D_UP;
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
    dMove = direct;
    if (!checkPassable())
    {
        return false;
    }
    return mainMove();    
};

void IMovable::ProcessForce()
{
    Dir step = VDirToDir(force_);
    VDir vstep = DirToVDir[step];
    force_.x -= vstep.x;
    force_.y -= vstep.y;
    force_.z -= vstep.z;

    checkMove(step);
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
    lastMove = static_cast<int>(MAIN_TICK);
    return true;
};

// TODO: ÒÎÐÍÀÄÎ
bool IMovable::checkPassable()
{
    if (!CanPass(owner->GetPassable(dMove), passable_level))
    {
        owner->Bump(GetId());
        return false;
    }

    auto neighbour = owner->GetNeighbour(dMove);
    if (   !CanPass(neighbour->GetPassable(D_ALL), passable_level)
        || !CanPass(neighbour->GetPassable(helpers::revert_dir(dMove)), passable_level))
    {
        neighbour->Bump(GetId());
        return false;
    }
    
    return true;
};

bool IMovable::mainMove()
{
    auto new_owner = owner->GetNeighbour(dMove);
    if (new_owner == owner)
        return false;

    owner->RemoveItem(GetId());
    new_owner->AddItem(GetId());

    if(new_owner->IsVisibleByPlayer())
    {
        Move* eff = EffectFabricOf<Move>::getEffectOf();
        eff->Init(TITLE_SIZE, dMove, pixSpeed, GetId(), true);
        eff->Start();
    }
    return true;
};

void IMovable::processImage(DrawType type)
{
    if (NODRAW)
        return;
    if (!GetSprite() || GetSprite()->Fail() || !GetMetadata())
        return;

    if (GetMetadata()->dirs >= 4)
    {
        DrawMain(helpers::dir_to_byond(dMove),            
            GetDrawX() + mob_position::get_shift_x(),
            GetDrawY() + mob_position::get_shift_y());
    }
    else
    {
        DrawMain(0,            
            GetDrawX() + mob_position::get_shift_x(),
            GetDrawY() + mob_position::get_shift_y());
    }
}

void IMovable::Bump(id_ptr_on<IMovable> item)
{
    id_ptr_on<IMob> m;
    m = item;
    if (m)
        checkMove(m->dMove);
}

bool IMovable::IsTransp(int x, int y)
{
    if (NODRAW)
        return true;
    if (!GetSprite() || GetSprite()->Fail() || !GetMetadata())
        return true;

    int shift = 0;
    if (GetMetadata()->dirs >= 4)
        shift = helpers::dir_to_byond(dMove);

    return view_.IsTransp(x, y, shift);
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

void ForceManager::Process()
{
    for (auto movable = to_add_.begin(); movable != to_add_.end(); ++movable)
    {
        if (!(*movable))
            continue;
        if (!NonZero((*movable)->force_))
            continue;

        under_force_.push_back(*movable);
    }

    std::vector<id_ptr_on<IMovable>> to_remove;

    for (auto movable = under_force_.begin(); movable != under_force_.end(); ++movable)
    {
        if (   !(*movable)
            || !NonZero((*movable)->force_))
        {
            to_remove.push_back(*movable);
            continue;
        }
        (*movable)->ProcessForce();
    }
    for (auto it = to_remove.begin(); it != to_remove.end(); ++it)
        under_force_.erase(std::find(under_force_.begin(), under_force_.end(), *it));
}