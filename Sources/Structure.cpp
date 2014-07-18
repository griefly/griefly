#include "Structure.h"

#include "Item.h"
#include "sound.h"

void Structure::AttackBy(id_ptr_on<Item> item)
{
    id_ptr_on<Screwdriver> scr;
    scr = item;
    if (scr)
    {
        anchored = !anchored;
        PlaySoundIfVisible("Screwdriver.ogg", owner.ret_id());
    }
}