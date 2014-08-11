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

void IMovable::move(Dir direct)
{
    assert(false);
    // TODO: remove
};

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
    if (   !CanPass(owner->GetNeighbour(dMove)->GetPassable(D_ALL), passable_level)
        || !CanPass(owner->GetNeighbour(dMove)->GetPassable(helpers::revert_dir(dMove)), passable_level))
    {
        owner->GetNeighbour(dMove)->Bump(GetId());
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