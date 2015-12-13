#include "Structure.h"

#include "Item.h"
#include "Sound.h"

void Structure::AttackBy(id_ptr_on<Item> item)
{
    if (id_ptr_on<Screwdriver> scr = item)
    {
        anchored = !anchored;
        PlaySoundIfVisible("Screwdriver.ogg", owner.ret_id());
    }
}