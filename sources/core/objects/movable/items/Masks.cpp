#include "Masks.h"

using namespace kv;

Mask::Mask()
{
    SetSprite("icons/masks.dmi");
    SetState("gas_mask");

    SetItemType(SlotType::MASK);

    SetName("Gas mask");
}
