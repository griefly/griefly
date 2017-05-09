#include "Lattice.h"

#include "FloorTile.h"
#include "Turf.h"
#include "Floor.h"

#include "../ObjectFactory.h"
#include "../Game.h"
#include "representation/Sound.h"

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
        Create<ITurf>(Plating::GetTypeStatic(), GetOwner());

        PlaySoundIfVisible("Deconstruct.wav");

        Delete();
    }
}
