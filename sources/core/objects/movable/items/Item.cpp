#include "Item.h"

#include "core/SynchronizedRandom.h"
#include "core/Game.h"

#include "core/objects/Slot.h"

using namespace kv;

Item::Item()
{
    passable_level = passable::SMALL_ITEM;

    type = Slots::DEFAULT;

    damage = 0;

    v_level = 5;
    SetSprite("icons/items.dmi");
}

Screwdriver::Screwdriver()
{
    damage = 5;
    SetState("screwdriver");
    name = "Screwdriver";
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

    name = "Wirecutters";
}

Crowbar::Crowbar()
{
    SetState("crowbar");

    damage = 15;

    name = "Crowbar";
}

Wrench::Wrench()
{
    SetState("wrench");

    damage = 5;

    name = "Wrench";
}
BulletCasing::BulletCasing()
{
    SetSprite("icons/ammo.dmi");
    SetState("s-casing");

    damage = 0;

    name = "Bullet Casing";
}
