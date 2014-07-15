#include "Glass.h"

#include "sync_random.h"
#include "helpers.h"
#include "MobInt.h"

FlatGlass::FlatGlass()
{
    transparent = true;

    dMove = get_rand() % 4;
    SetPassable(dMove, false);

    tickSpeed = 5;
    pixSpeed = 1;

    v_level = 9;

    SetSprite("icons/structures.dmi");
    SetState("window");
    
    name = "Window";
}

bool FlatGlass::checkMove(Dir direct)
{
    SetPassable(dMove, true);
    bool retval = IMovable::checkMove(direct);
    SetPassable(dMove, false);
    return retval;
}

void FlatGlass::Bump(id_ptr_on<IMovable> item)
{
    id_ptr_on<IMob> m;
    m = item;
    if (!m)
        return;

    if (item->dMove != dMove)
    {
        if (!owner->IsPassable(D_ALL))
            return;
        SetPassable(dMove, true);
        dMove = item->dMove;
        SetPassable(dMove, false);
        return;
    }
    IMovable::Bump(item);
}


ReinforcedFlatGlass::ReinforcedFlatGlass()
{
    SetState("rwindow");
    name = "Reinforced window";
}