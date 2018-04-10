#include "Girder.h"

#include "objects/movable/items/Materials.h"
#include "objects/turfs/MetalWall.h"

#include "ObjectFactory.h"

using namespace kv;

Girder::Girder()
{
    SetSprite("icons/structures.dmi");
    SetState("girder");

    SetPassable(Dir::ALL, passable::AIR);

    SetAnchored(true);
    SetName("Girder");
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
