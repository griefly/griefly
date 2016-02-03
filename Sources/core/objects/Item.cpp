#include "Item.h"

#include <sstream>

#include "../SyncRandom.h"
#include "Slot.h"

Item::Item(size_t id) : IMovable(id)
{
    passable_level = Passable::SMALL_ITEM;

    type = Slots::DEFAULT;

    damage = 0;

    v_level = 5;
    SetSprite("icons/items.dmi");
}

Screwdriver::Screwdriver(size_t id) : Item(id)
{
    unsigned int value = get_rand() % 8;

    damage = 5;

    std::stringstream converter;
    converter << "screwdriver";
    if (value > 1)
    {
        converter << value;
    }
    SetState(converter.str());
    
    name = "Screwdriver";
}

Wirecutters::Wirecutters(size_t id) : Item(id)
{
    SetState("cutters");

    damage = 10;

    name = "Wirecutters";
}

Crowbar::Crowbar(size_t id) : Item(id)
{
    SetState("crowbar");

    damage = 15;

    name = "Crowbar";
}

Wrench::Wrench(size_t id) : Item(id)
{
    SetState("wrench");

    damage = 5;

    name = "Wrench";
}
