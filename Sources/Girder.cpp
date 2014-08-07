#include "Girder.h"

#include "Materials.h"
#include "MetalWall.h"

#include "ItemFabric.h"

Girder::Girder()
{
    SetSprite("icons/structures.dmi");
    SetState("girder");

    SetPassable(D_ALL, false);

    anchored = true;
    name = "Girder";
}

void Girder::AttackBy(id_ptr_on<Item> item)
{
    if (id_ptr_on<Metal> metal = item)
    {
        metal->delThis();

        GetTurf()->delThis();
        SetTurf(GetItemFabric()->newItem<ITurf>(MetalWall::T_ITEM_S()));

        delThis();
    }
}