#include "Lattice.h"

#include "FloorTile.h"
#include "Turf.h"
#include "Floor.h"

#include "ItemFabric.h"
#include "sound.h"

Lattice::Lattice(size_t id) : Structure(id)
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

        GetTurf()->delThis();
        SetTurf(GetFactory().Create<ITurf>(Floor::T_ITEM_S()));

        PlaySoundIfVisible("Deconstruct.ogg", owner.ret_id());

        delThis();
    }
}
