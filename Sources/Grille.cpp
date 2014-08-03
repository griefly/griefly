#include "Grille.h"

#include "MobInt.h"
#include "Item.h"
#include "sound.h"

Grille::Grille()
{
    transparent = true;
    SetPassable(D_ALL, false);

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
            SetPassable(D_ALL, true);
            cutted_ = true;
        }
        else
        {
            delThis();
        }
    }
    else
        Structure::AttackBy(item);
}