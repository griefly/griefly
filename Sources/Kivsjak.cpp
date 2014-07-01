#include "Kivsjak.h"
#include "LiquidHolder.h"
#include "MapClass.h"
#include "Turf.h"
#include "sync_random.h"
#include "CSmallItem.h"
#include "ItemFabric.h"

Kivsjak::Kivsjak()
{
    SetSprite("icons/kivsjak.png");
    imageStateW = dMove;
    v_level = 9;
    tickSpeed = 1;
    max_dmg = 300;
    passable = false;
    food = 0;
    name = "Kivsjak";
}

void Kivsjak::aaMind()
{
    if(get_rand() % 13 == 0)
        checkMove(get_rand() % 4);
    auto it = owner->GetItem<CWeed>();
    if(it.valid())
    {
        (*it)->delThis();
        food += 10;
    }
    if(food > 250)
    {
        food = 0;
        //map->newItemOnMap<IOnMapItem>(hash("kivsjak"), posx, posy);
    }
}

void Kivsjak::live()
{
    CAliveMob::live();
    if(dmg + bloodless + burn + interior + oxyless > max_dmg)
    {
        GetItemFabric()->newItemOnMap<IOnMapItem>(hash("meat"), owner);
        delThis();
    }

    if (!GetOwner()->GetTurf().valid()) {};//SYSTEM_STREAM << "Its ok" << std::endl;
}

bool Kivsjak::checkMove(Dir direct)
{
    bool retval = CAliveMob::checkMove(direct);
    imageStateW = dMove;
    return retval;
}