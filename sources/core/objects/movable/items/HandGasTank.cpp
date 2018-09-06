#include "HandGasTank.h"

using namespace kv;

HandGasTank::HandGasTank()
{
    SetSprite("icons/tank.dmi");
    SetState("oxygen");

    SetItemType(SlotType::BACK);

    SetName("Gas tank");

    atmos_holder_.AddGase(atmos::OXYGEN, 1000);
}
