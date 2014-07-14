#include "IMovable.h"

#include "EffectSystem.h"
#include "MoveEffect.h"
#include "helpers.h"
#include "MobInt.h"

#include <assert.h>

IMovable::IMovable()
{
    lastMove = 0;
    tickSpeed = 1;
    pixSpeed = 1;
    dMove = D_UP;
}

bool IMovable::checkMove(Dir direct)
{
    if (!checkMoveTime()) 
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
    if (!owner->IsPassable(dMove))
    {
        owner->Bump(GetId());
        return false;
    }
    if (   !owner->GetNeighbour(dMove)->IsPassable(D_ALL)
        || !owner->GetNeighbour(dMove)->IsPassable(helpers::revert_dir(dMove)))
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
        DrawMain(helpers::dir_to_byond(dMove));
    else
        DrawMain(0);
}

void IMovable::Bump(id_ptr_on<IMovable> item)
{
    id_ptr_on<IMob> m;
    m = item;
    if (m)
        checkMove(m->dMove);
}