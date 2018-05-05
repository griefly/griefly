#include "Item.h"

#include "SynchronizedRandom.h"

#include "objects/movable/Tables.h"
#include "objects/mobs/Human.h"

using namespace kv;

Item::Item()
{
    SetPassableLevel(passable::SMALL_ITEM);

    SetItemType(SlotType::DEFAULT);

    SetDamage(0);

    SetVisibleLevel(visible_level::ITEM);
    SetSprite("icons/items.dmi");
}

bool Item::Attack(IdPtr<MapObject> object)
{
    if (IdPtr<Table> table = object)
    {
        // TODO: remove when RemoveItem will be implemented properly in Human
        if (IdPtr<Human> human = GetOwner())
        {
            table->GetOwner()->AddObject(GetId());
            human->GetHumanInterface()->DropItem();
            human->UpdateOverlays();
            return false;
        }
    }
    return true;
}

Screwdriver::Screwdriver()
{
    SetDamage(5);
    SetState("screwdriver");
    SetName("Screwdriver");
}

void Screwdriver::AfterWorldCreation()
{
    Item::AfterWorldCreation();

    unsigned int value = GenerateRandom() % 8;
    SetState(QString("screwdriver%1").arg(value));
}

Wirecutters::Wirecutters()
{
    SetState("cutters");

    SetDamage(10);

    SetName("Wirecutters");
}

Crowbar::Crowbar()
{
    SetState("crowbar");

    SetDamage(15);

    SetName("Crowbar");
}

Wrench::Wrench()
{
    SetState("wrench");

    SetDamage(5);

    SetName("Wrench");
}
BulletCasing::BulletCasing()
{
    SetSprite("icons/ammo.dmi");
    SetState("s-casing");

    SetDamage(0);

    SetName("Bullet Casing");
}
