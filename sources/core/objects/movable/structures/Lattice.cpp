#include "Lattice.h"

#include "objects/movable/items/FloorTile.h"
#include "objects/turfs/Turf.h"
#include "objects/turfs/Floor.h"

#include "ObjectFactory.h"

using namespace kv;

Lattice::Lattice()
{
    SetSprite("icons/structures.dmi");
    SetState("latticefull");

    SetVisibleLevel(1);
    anchored_ = true;

    SetName("Lattice");
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
