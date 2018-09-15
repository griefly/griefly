#include "HandGasTank.h"

#include "Limbs.h"
#include "Masks.h"
#include "objects/mobs/Human.h"

using namespace kv;

HandGasTank::HandGasTank()
{
    SetSprite("icons/tank.dmi");
    SetState("oxygen");

    SetItemType(SlotType::BACK);

    SetName("Gas tank");

    atmos_holder_.AddGase(atmos::OXYGEN, 1000);
}

void HandGasTank::AttackBy(IdPtr<Item> item)
{
    if (IdPtr<Hand> hand = item)
    {
        if (IdPtr<Human> human = GetOwner())
        {
            IdPtr<Item> mask = human->GetHumanInterface()->GetItem(slot::MASK);
            if (IdPtr<GasMask> gas_mask = mask)
            {
                gas_mask->SetGasTank(GetId());
                PostHtmlFor("Gas tank has been connected to the mask!", human);
            }
        }
    }
    Item::AttackBy(item);
}
