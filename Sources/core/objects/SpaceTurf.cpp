#include "SpaceTurf.h"

#include "../SyncRandom.h"

#include "Item.h"
#include "Materials.h"
#include "Lattice.h"
#include "../ObjectFactory.h"

Space::Space(size_t id) : ITurf(id)
{
    SetAtmosState(SPACE);

    SetPassable(D_ALL, Passable::FULL);
    transparent = true;
    
    SetFriction(0);

    SetSprite("icons/space.png"); 
    std::stringstream conv;
    size_t value = get_rand() % 25 + 1;
    conv << value;
    SetState(conv.str());
    name = "Space";
}

void Space::AttackBy(id_ptr_on<Item> item)
{
    if (id_ptr_on<Rod> rod = item)
    {
        rod->Delete();
        GetFactory().Create<IOnMapObject>(Lattice::T_ITEM_S(), GetOwner());
    }
}
