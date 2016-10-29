#include "Grille.h"

#include "Mob.h"
#include "Item.h"
#include "representation/Sound.h"

#include "../ObjectFactory.h"
#include "../Game.h"
#include "Materials.h"

Grille::Grille(quint32 id) : Structure(id)
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
}

void Grille::AttackBy(IdPtr<Item> item)
{
    if (IdPtr<Wirecutters> w = item)
    {
        PlaySoundIfVisible("Wirecutter.ogg", owner.Id());
        if (!cutted_)
        {
            SetState("brokengrille");
            SetPassable(D_ALL, Passable::FULL);
            cutted_ = true;
            Create<IOnMapObject>(Rod::T_ITEM_S(), GetOwner());
        }
        else
        {
            Create<IOnMapObject>(Rod::T_ITEM_S(), GetOwner());
            Delete();
        }
    }
    else
    {
        Structure::AttackBy(item);
    }
}
