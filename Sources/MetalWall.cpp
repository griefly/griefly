#include "MetalWall.h"

#include "Weldingtool.h"
#include "Girder.h"
#include "Floor.h"
#include "Materials.h"

#include "sound.h"

#include "ItemFabric.h"

MetalWall::MetalWall()
{
    transparent = false;
    SetPassable(D_ALL, Passable::EMPTY);

    SetSprite("icons/walls.dmi");
    SetState("metal0");

    name = "Metal wall";
}

void MetalWall::AttackBy(id_ptr_on<Item> item)
{
    if (id_ptr_on<Weldingtool> wtool = item)
    {
        if (wtool->Working())
        {
            PlaySoundIfVisible("Welder.ogg", owner.ret_id());
            GetItemFabric()->newItemOnMap<IOnMapObject>(Girder::T_ITEM_S(), GetOwner());
            GetItemFabric()->newItemOnMap<IOnMapObject>(Metal::T_ITEM_S(), GetOwner());
            SetTurf(GetItemFabric()->newItem<ITurf>(Floor::T_ITEM_S()));
            delThis();
        }
    }
}

ReinforcedWall::ReinforcedWall()
{
    transparent = false;
    SetPassable(D_ALL, Passable::EMPTY);

    SetSprite("icons/walls.dmi");
    SetState("r_wall");

    name = "Reinforced wall";
}