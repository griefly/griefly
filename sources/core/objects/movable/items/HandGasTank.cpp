#include "HandGasTank.h"

using namespace kv;

HandGasTank::HandGasTank()
{
    SetSprite("icons/tank.dmi");
    SetState("oxygen");

    SetItemType(SlotType::BACK);

    SetName("Gas tank");
}
