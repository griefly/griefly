#include "Lattice.h"

#include "core/objects/movable/items/FloorTile.h"
#include "core/objects/turfs/Turf.h"
#include "core/objects/turfs/Floor.h"

#include "core/ObjectFactory.h"

using namespace kv;

Lattice::Lattice()
{
    SetSprite("icons/structures.dmi");
    SetState("latticefull");

    v_level = 1;
    anchored_ = true;

    name = "Lattice";
}

void Lattice::AttackBy(IdPtr<Item> item)
{
    if (IdPtr<FloorTile> tile = item)
    {
        tile->Delete();

        GetTurf()->Delete();
        Create<Turf>(Plating::GetTypeStatic(), GetOwner());

        PlaySoundIfVisible("Deconstruct.wav");

        Delete();
    }
}
