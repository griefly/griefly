#include "Item.h"

#include "SynchronizedRandom.h"

using namespace kv;

Item::Item()
{
    SetPassableLevel(passable::SMALL_ITEM);

    type = SlotType::DEFAULT;

    damage = 0;

    v_level = 5;
    SetSprite("icons/items.dmi");
}

Screwdriver::Screwdriver()
{
    damage = 5;
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

    damage = 10;

    SetName("Wirecutters");
}

Crowbar::Crowbar()
{
    SetState("crowbar");

    damage = 15;

    SetName("Crowbar");
}

Wrench::Wrench()
{
    SetState("wrench");

    damage = 5;

    SetName("Wrench");
}
BulletCasing::BulletCasing()
{
    SetSprite("icons/ammo.dmi");
    SetState("s-casing");

    damage = 0;

    SetName("Bullet Casing");
}
