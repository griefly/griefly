#include "Floor.h"

#include "sound.h"

#include "Item.h"

Floor::Floor()
{
    transparent = true;
    SetPassable(D_ALL, true);

    SetSprite("icons/floors.dmi");
    SetState("floor");

    name = "Floor";

    open_ = false;
}

void Floor::AttackBy(id_ptr_on<Item> item)
{
    if (id_ptr_on<Crowbar> c = item)
    {
        if (!open_)
        {
            SetState("plating");
            open_ = true;
            PlaySoundIfVisible("Crowbar.ogg", owner.ret_id());
        }
    }
}