#include "GasTank.h"

#include "Tile.h"
#include "representation/Chat.h"
#include "../Game.h"

#include "ElectricTools.h"

const int O2_TANK_AMOUNT = 10000;
const int O2_TANK_ENERGY = 5000;

GasTank::GasTank(quint32 id)
    : IMovable(id)
{
    name = "Oxygen tank";

    v_level = 5;

    SetPassable(D_ALL, Passable::AIR);

    SetSprite("icons/atmos.dmi");
    SetState("blue");

    open_ = false;
    atmos_holder_.AddGase(atmos::OXYGEN, O2_TANK_AMOUNT);
    atmos_holder_.AddEnergy(O2_TANK_ENERGY);
    // atmos_holder_.SetVolume();
}

void GasTank::AfterWorldCreation()
{
    IMovable::AfterWorldCreation();
    SetFreq(1);
}

void GasTank::AttackBy(IdPtr<Item> item)
{
    if (IdPtr<AtmosTool> at = item)
    {
        GetGame().GetChat().PostHtmlFor(AtmosTool::GetHtmlInfo(atmos_holder_), at->GetOwner());
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
    GetGame().GetChat().PostSimpleText(name + " is open", owner.Id());

    open_ = true;
}

void GasTank::Close()
{
    GetGame().GetChat().PostSimpleText(name + " is closed", owner.Id());

    open_ = false;
}

void GasTank::Process()
{
    if (!open_)
    {
        return;
    }
    if (IdPtr<CubeTile> ct = owner)
    {
        atmos_holder_.Connect(ct->GetAtmosHolder());
    }
}

MagicGasTank::MagicGasTank(quint32 id) : GasTank(id)
{
    name = "Magic tank";
}

void MagicGasTank::AfterWorldCreation()
{
    GasTank::AfterWorldCreation();
    SetFreq(10);
}

void MagicGasTank::Process()
{
    GasTank::Process();

    GetAtmosHolder()->RemoveGase(atmos::CO2, GetAtmosHolder()->GetGase(atmos::CO2));
    GetAtmosHolder()->RemoveGase(atmos::NYTROGEN, GetAtmosHolder()->GetGase(atmos::NYTROGEN));

    int new_oxygen = qMax(0, O2_TANK_AMOUNT - GetAtmosHolder()->GetGase(atmos::OXYGEN));
    GetAtmosHolder()->AddGase(atmos::OXYGEN, new_oxygen);

    int new_energy = qMax(0, O2_TANK_ENERGY - GetAtmosHolder()->GetEnergy());
    GetAtmosHolder()->AddEnergy(new_energy);
}
