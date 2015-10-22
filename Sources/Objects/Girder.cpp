#include "Girder.h"

#include "Materials.h"
#include "MetalWall.h"
#include "Item.h"

#include "ItemFabric.h"

#include "sound.h"

Girder::Girder(size_t id) : Structure(id)
{
    SetSprite("icons/structures.dmi");
    SetState("girder");

    SetPassable(D_ALL, Passable::AIR);

    anchored = true;
    name = "Girder";
}

void Girder::AttackBy(id_ptr_on<Item> item)
{
    if (id_ptr_on<Metal> metal = item)
    {
        metal->delThis();

        GetTurf()->delThis();
        SetTurf(GetItemFabric().newItem<ITurf>(MetalWall::T_ITEM_S()));

        delThis();
    }
    else if (id_ptr_on<Wrench> wrench = item)
    {
        PlaySoundIfVisible("Ratchet.ogg", owner.ret_id());
        GetItemFabric().newItemOnMap<Item>(Metal::T_ITEM_S(), GetOwner());
        delThis();
    }
}
