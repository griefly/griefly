#include "MetalWall.h"

#include "core/objects/movable/items/Weldingtool.h"
#include "core/objects/movable/structures/Girder.h"
#include "Floor.h"
#include "core/objects/movable/items/Materials.h"

#include "representation/Sound.h"

#include "core/ObjectFactory.h"
#include "core/Game.h"

using namespace kv;

MetalWall::MetalWall()
{
    transparent = false;
    SetPassable(Dir::ALL, passable::EMPTY);

    v_level = 2;

    SetSprite("icons/walls.dmi");
    SetState("metal0");

    name = "Metal wall";
}

void MetalWall::AttackBy(IdPtr<Item> item)
{
    if (IdPtr<Weldingtool> wtool = item)
    {
        if (wtool->Working())
        {
            PlaySoundIfVisible("Welder.wav");
            Create<MaterialObject>(Girder::GetTypeStatic(), GetOwner());
            Create<MaterialObject>(Metal::GetTypeStatic(), GetOwner());
            Create<Turf>(Plating::GetTypeStatic(), GetOwner());
            Delete();
        }
    }
}

ReinforcedWall::ReinforcedWall()
{
    transparent = false;
    SetPassable(Dir::ALL, passable::EMPTY);

    v_level = 2;

    SetSprite("icons/walls.dmi");
    SetState("r_wall");

    name = "Reinforced wall";
}
