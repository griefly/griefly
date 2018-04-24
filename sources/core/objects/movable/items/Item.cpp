#include "Item.h"

#include "SynchronizedRandom.h"

using namespace kv;

Item::Item()
{
    SetPassableLevel(passable::SMALL_ITEM);

    SetItemType(SlotType::DEFAULT);

    SetDamage(0);

    SetVisibleLevel(visible_level::ITEM);
    SetSprite("icons/items.dmi");
}

void Item::Attack(IdPtr<MapObject> /*object*/)
{
    // Nothing
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
