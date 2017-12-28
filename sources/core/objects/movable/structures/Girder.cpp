#include "Girder.h"

#include "core/objects/movable/items/Materials.h"
#include "core/objects/turfs/MetalWall.h"

#include "core/ObjectFactory.h"

using namespace kv;

Girder::Girder()
{
    SetSprite("icons/structures.dmi");
    SetState("girder");

    SetPassable(Dir::ALL, passable::AIR);

    anchored_ = true;
    name = "Girder";
}

void Girder::AttackBy(IdPtr<Item> item)
{
    if (IdPtr<Metal> metal = item)
    {
        metal->Delete();

        GetTurf()->Delete();
        Create<Turf>(MetalWall::GetTypeStatic(), GetOwner());

        Delete();
    }
    else if (IdPtr<Wrench> wrench = item)
    {
        PlaySoundIfVisible("Ratchet.wav");
        Create<Item>(Metal::GetTypeStatic(), GetOwner());
        Delete();
    }
}
