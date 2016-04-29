#include "Lattice.h"

#include "FloorTile.h"
#include "Turf.h"
#include "Floor.h"

#include "../ObjectFactory.h"
#include "../Game.h"
#include "representation/Sound.h"

Lattice::Lattice(size_t id) : Structure(id)
{
    SetSprite("icons/structures.dmi");
    SetState("latticefull");

    v_level = 1;

    name = "Lattice";
}

void Lattice::AttackBy(id_ptr_on<Item> item)
{
    if (id_ptr_on<FloorTile> tile = item)
    {
        tile->Delete();

        GetTurf()->Delete();
        GetGame().GetFactory().Create<ITurf>(Plating::T_ITEM_S(), GetOwner());

        PlaySoundIfVisible("Deconstruct.ogg", owner.ret_id());

        Delete();
    }
}
