#include "Grille.h"

#include "Mob.h"
#include "Item.h"
#include "sound.h"

#include "ItemFabric.h"
#include "Materials.h"

Grille::Grille(size_t id) : Structure(id)
{
    transparent = true;
    SetPassable(D_ALL, Passable::AIR);

    tickSpeed = 5;
    pixSpeed = 1;

    v_level = 8;

    SetSprite("icons/structures.dmi");
    SetState("grille");

    name = "Grille";

    cutted_ = false;
};

void Grille::AttackBy(id_ptr_on<Item> item)
{
    if (id_ptr_on<Wirecutters> w = item)
    {
        PlaySoundIfVisible("Wirecutter.ogg", owner.ret_id());
        if (!cutted_)
        {
            SetState("brokengrille");
            SetPassable(D_ALL, Passable::FULL);
            cutted_ = true;
            GetFactory().Create<IOnMapObject>(Rod::T_ITEM_S(), GetOwner());
        }
        else
        {
            GetFactory().Create<IOnMapObject>(Rod::T_ITEM_S(), GetOwner());
            delThis();
        }
    }
    else
        Structure::AttackBy(item);
}
