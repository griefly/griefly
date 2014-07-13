#include "IMovable.h"

#include "EffectSystem.h"
#include "MoveEffect.h"
#include "helpers.h"

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
        owner->GetNeighbour(dMove)->Bump(GetId());
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
    if(MAIN_TICK - lastMove < tickSpeed) 
        return false;
    lastMove = static_cast<int>(MAIN_TICK);
    return true;
};

// TODO: ÒÎÐÍÀÄÎ
bool IMovable::checkPassable()
{
    return owner->GetNeighbour(dMove)->IsPassable();
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

    if (GetMetadata()->dirs == 4)
        DrawMain(helpers::dir_to_byond(dMove));
    else
        DrawMain(0);
}