#include "GasTank.h"

#include "Tile.h"
#include "representation/Chat.h"

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
    SetFreq(1);
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
}

void GasTank::Close()
{
    GetChat().PostSimpleText(name + " is closed", owner.ret_id());

    open_ = false;
}

void GasTank::Process()
{
    if (!open_)
    {
        return;
    }
    if (id_ptr_on<CubeTile> ct = owner)
    {
        atmos_holder_.Connect(ct->GetAtmosHolder());
    }
}

MagicGasTank::MagicGasTank(size_t id) : GasTank(id)
{
    name = "Magic tank";
    SetFreq(10);
}

void MagicGasTank::Process()
{
    GasTank::Process();

    GetAtmosHolder()->RemoveGase(CO2, GetAtmosHolder()->GetGase(CO2));
    GetAtmosHolder()->RemoveGase(NYTROGEN, GetAtmosHolder()->GetGase(NYTROGEN));

    int new_oxygen = std::max(0, 20000 - static_cast<int>(GetAtmosHolder()->GetGase(OXYGEN)));
    GetAtmosHolder()->AddGase(OXYGEN, new_oxygen);

    int new_energy = std::max(0, 10000 - static_cast<int>(GetAtmosHolder()->GetEnergy()));
    GetAtmosHolder()->AddEnergy(new_energy);
}
