#include "Floor.h"

#include "sound.h"

#include "Item.h"
#include "FloorTile.h"

#include "ItemFabric.h"

Floor::Floor(size_t id) : ITurf(id)
{
    transparent = true;
    SetPassable(D_ALL, Passable::FULL);

    SetSprite("icons/floors.dmi");
    SetState("floor");

    name = "Floor";

    SetOpen(true);
}

void Floor::AttackBy(id_ptr_on<Item> item)
{
    if (id_ptr_on<Crowbar> c = item)
    {
        if (!open_)
        {
            SetOpen(true);
            GetItemFabric()->newItemOnMap<Item>(FloorTile::T_ITEM_S(), GetOwner());
            PlaySoundIfVisible("Crowbar.ogg", owner.ret_id());
        }
    }
    else if (id_ptr_on<FloorTile> ftile = item)
    {
        if (open_)
        {
            SetOpen(false);
            ftile->delThis();
            PlaySoundIfVisible("Deconstruct.ogg", owner.ret_id());
        }
    }
}

void Floor::SetOpen(bool o)
{
    open_ = o;
    if (open_)
    {
        SetState("plating");
    }
    else
    {
        SetState("floor");
    }
}
