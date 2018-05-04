#include "GasTank.h"

#include "objects/Tile.h"

#include "objects/movable/items/ElectricTools.h"
#include "objects/movable/items/Limbs.h"

using namespace kv;

namespace
{
    const int O2_TANK_AMOUNT = 10000;
    const int O2_TANK_ENERGY = 5000;

    const int PLASMA_TANK_AMOUNT = 10000;

    const int BASE_PRESSURE = 160000;
}

GasTank::GasTank()
{
    SetName("Oxygen tank");

    SetVisibleLevel(visible_level::ITEM);

    SetPassable(Dir::ALL, passable::AIR);

    SetSprite("icons/atmos.dmi");
    SetState("blue");

    state_ = State::CLOSED;
    atmos_holder_.AddGase(atmos::OXYGEN, O2_TANK_AMOUNT);
    atmos_holder_.AddEnergy(O2_TANK_ENERGY);
}

void GasTank::AfterWorldCreation()
{
    Movable::AfterWorldCreation();
    SetFreq(1);
}

void GasTank::AttackBy(IdPtr<Item> item)
{
    IdPtr<Hand> hand = item;
    if (!hand.IsValid())
    {
        return;
    }

    if (state_ == State::BROKEN)
    {
        return;
    }

    if (state_ == State::OPEN)
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
    PostVisible(GetName() + " is open", GetPosition());

    state_ = State::OPEN;
}

void GasTank::Close()
{
    PostVisible(GetName() + " is closed", GetPosition());

    state_ = State::CLOSED;
}

void GasTank::Break()
{
    if (state_ == State::BROKEN)
    {
        return;
    }

    PostVisible(GetName() + " breaks apart!", GetPosition());
    SetState(GetView().GetBaseFrameset().GetState() + "-1");

    state_ = State::BROKEN;
}

namespace
{

const int OVERLAYS_SIZE = 4;
const QString OVERLAYS_STATES[OVERLAYS_SIZE] = {"can-o0", "can-o1", "can-o2", "can-o3"};

}

void GasTank::Process()
{
    GetView().RemoveOverlays();

    const int pressure = GetAtmosHolder()->GetPressure();
    const int overlay_id = std::min(std::max((pressure * OVERLAYS_SIZE) / BASE_PRESSURE, 0), OVERLAYS_SIZE - 1);
    GetView().AddOverlay("icons/atmos.dmi", OVERLAYS_STATES[overlay_id]);

    if (state_ == State::CLOSED)
    {
        return;
    }
    if (IdPtr<CubeTile> tile = GetOwner())
    {
        atmos_holder_.Connect(tile->GetAtmosHolder());
    }
}

void GasTank::ApplyFire(int intensity)
{
    if (state_ == State::BROKEN)
    {
        return;
    }
    if ((GenerateRandom() % 8) == 0)
    {
        Break();
    }
}

MagicGasTank::MagicGasTank()
{
    SetName("Magic tank");
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

    const int new_oxygen = std::max(0, O2_TANK_AMOUNT - GetAtmosHolder()->GetGase(atmos::OXYGEN));
    GetAtmosHolder()->AddGase(atmos::OXYGEN, new_oxygen);

    const int new_energy = std::max(0, O2_TANK_ENERGY - GetAtmosHolder()->GetEnergy());
    GetAtmosHolder()->AddEnergy(new_energy);
}

PlasmaGasTank::PlasmaGasTank()
{
    SetName("Plasma tank");
    SetState("orange");

    GetAtmosHolder()->RemoveGase(atmos::OXYGEN, O2_TANK_AMOUNT);
    GetAtmosHolder()->AddGase(atmos::PLASMA, PLASMA_TANK_AMOUNT);
}
