#include "MetalWall.h"

#include "Weldingtool.h"
#include "Girder.h"
#include "Floor.h"
#include "Materials.h"

#include "representation/Sound.h"

#include "../ObjectFactory.h"

MetalWall::MetalWall(size_t id) : ITurf(id)
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
            GetFactory().Create<IOnMapObject>(Girder::T_ITEM_S(), GetOwner());
            GetFactory().Create<IOnMapObject>(Metal::T_ITEM_S(), GetOwner());
            SetTurf(GetFactory().Create<ITurf>(Floor::T_ITEM_S()));
            delThis();
        }
    }
}

ReinforcedWall::ReinforcedWall(size_t id) : ITurf(id)
{
    transparent = false;
    SetPassable(D_ALL, Passable::EMPTY);

    SetSprite("icons/walls.dmi");
    SetState("r_wall");

    name = "Reinforced wall";
}
