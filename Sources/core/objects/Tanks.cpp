#include "Tanks.h"

#include "Weldingtool.h"
#include "representation/Sound.h"

FuelTank::FuelTank(size_t id)
    : Tank(id)
{
    name = "Fueltank";

    v_level = 6;

    transparent = true;

    SetSprite("icons/objects.dmi");
    SetState("weldtank");

    SetPassable(D_ALL, Passable::AIR);
}

void FuelTank::AttackBy(id_ptr_on<Item> item)
{
    if (id_ptr_on<Weldingtool> w = item)
    {
        w->AddFuel(50);
        PlaySoundIfVisible("refill.ogg", owner.ret_id());
    }
}

WaterTank::WaterTank(size_t id)
    : Tank(id)
{
    name = "Watertank";

    v_level = 6;

    transparent = true;

    SetSprite("icons/objects.dmi");
    SetState("watertank");

    SetPassable(D_ALL, Passable::AIR);
}

void WaterTank::AttackBy(id_ptr_on<Item> item)
{

}

