#include "Materials.h"

#include "Girder.h"
#include "ItemFabric.h"
#include "Floor.h"

Rod::Rod()
{
    SetState("rods");

    name = "Rods";
}

Metal::Metal()
{
    SetState("sheet-metal");

    name = "Metal";
}

void Metal::AttackBy(id_ptr_on<Item> item)
{
    if (!item) // Attack self
    {
        if (id_ptr_on<Floor> floor = GetTurf())
        {
            GetItemFabric()->newItemOnMap<IOnMapObject>(Girder::T_ITEM_S(), GetTurf());
            delThis();
        }
    }
}
