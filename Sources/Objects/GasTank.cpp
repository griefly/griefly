#include "GasTank.h"

#include "Tile.h"
#include "Chat.h"

#include "ElectricTools.h"

GasTank::GasTank(size_t id)
    : IMovable(id)
{
    name = "Oxygen tank";

    v_level = 5;

    SetPassable(D_ALL, Passable::AIR);

    SetSprite("icons/atmos.dmi");
    SetState("blue");

    open_ = false;
    atmos_holder_.AddGase(OXYGEN, 20000);
    atmos_holder_.AddEnergy(10000);
   // atmos_holder_.SetVolume();
}

void GasTank::AttackBy(id_ptr_on<Item> item)
{
    if (id_ptr_on<AtmosTool> at = item)
    {
        GetChat().PostTextFor(AtmosTool::GetInfo(atmos_holder_), at->GetOwner());
        return;
    }

    if (item)
    {
        return;
    }

    if (open_)
    {
        Close();
    }
    else
    {
        Open();
    }

}

void GasTank::Open()
{
    GetChat().PostSimpleText(name + " is open", owner.ret_id());

    open_ = true;
    SetFreq(1);
}

void GasTank::Close()
{
    GetChat().PostSimpleText(name + " is closed", owner.ret_id());

    open_ = false;
    SetFreq(0);
}

void GasTank::process()
{
    if (id_ptr_on<CubeTile> ct = owner)
    {
        atmos_holder_.Connect(ct->GetAtmosHolder());
    }
}
