#include "Lattice.h"

#include "FloorTile.h"
#include "ITurf.h"
#include "Floor.h"

#include "ItemFabric.h"
#include "sound.h"

Lattice::Lattice()
{
    SetSprite("icons/structures.dmi");
    SetState("latticefull");

    name = "Lattice";
}

void Lattice::AttackBy(id_ptr_on<Item> item)
{
    if (id_ptr_on<FloorTile> tile = item)
    {
        tile->delThis();

        GetOwner()->GetTurf()->delThis();
        GetOwner()->SetTurf(GetItemFabric()->newItem<ITurf>(Floor::T_ITEM_S()));

        PlaySoundIfVisible("Deconstruct.ogg", owner.ret_id());

        delThis();
    }
}