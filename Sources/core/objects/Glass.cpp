#include "Glass.h"

#include "../SyncRandom.h"
#include "../Helpers.h"
#include "Mob.h"

FlatGlass::FlatGlass(size_t id) : Structure(id)
{
    transparent = true;

    tickSpeed = 5;
    pixSpeed = 1;

    v_level = 9;

    SetSprite("icons/structures.dmi");
    SetState("window");
    
    name = "Window";
}

void FlatGlass::AfterWorldCreation()
{
    Structure::AfterWorldCreation();

    SetPassable(D_UP, Passable::FULL);
    SetPassable(D_DOWN, Passable::FULL);
    SetPassable(D_LEFT, Passable::FULL);
    SetPassable(D_RIGHT, Passable::FULL);

    SetPassable(GetDir(), Passable::EMPTY);
}

void FlatGlass::Bump(id_ptr_on<IMovable> item)
{
    id_ptr_on<IMob> m;
    m = item;
    if (!m)
        return;

    if (item->GetDir() != GetDir())
    {
        if (!CanPass(owner->GetPassable(D_ALL), passable_level))
            return;
        if (anchored)
            return;

        Rotate(item->GetDir());
        return;
    }
    IMovable::Bump(item);
}

bool FlatGlass::Rotate(Dir dir)
{
    SetPassable(GetDir(), Passable::FULL);
    Structure::Rotate(dir);
    SetPassable(GetDir(), Passable::EMPTY);
    return true;
}

ReinforcedFlatGlass::ReinforcedFlatGlass(size_t id) : FlatGlass(id)
{
    SetState("rwindow");
    name = "Reinforced window";
}
