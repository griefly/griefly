#include "Structure.h"

#include "core/objects/movable/items/Item.h"
#include "representation/Sound.h"

using namespace kv;

void Structure::AttackBy(IdPtr<Item> item)
{
    if (IdPtr<Screwdriver> scr = item)
    {
        anchored_ = !anchored_;
        PlaySoundIfVisible("Screwdriver.wav");
    }
}
