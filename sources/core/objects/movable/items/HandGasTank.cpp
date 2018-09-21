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
        if (IdPtr<GasMask> mask = GetGasMask())
        {
            mask->SetGasTank(0);
            SetGasMask(0);
            if (IdPtr<Human> human = GetOwner())
            {
                PostHtmlFor("Gas tank has been disconnected from the mask!", human);
            }
            return;
        }
        if (IdPtr<Human> human = GetOwner())
        {
            IdPtr<Item> mask = human->GetHumanInterface()->GetItem(slot::MASK);
            if (IdPtr<GasMask> gas_mask = mask)
            {
                if (IdPtr<HandGasTank> gas_tank = gas_mask->GetGasTank())
                {
                    gas_tank->SetGasMask(0);
                }
                if (IdPtr<GasMask> mask = GetGasMask())
                {
                    mask->SetGasTank(0);
                }
                gas_mask->SetGasTank(GetId());
                SetGasMask(gas_mask);
                PostHtmlFor("Gas tank has been connected to the mask!", human);
                return;
            }
        }
    }
    Item::AttackBy(item);
}
