#include "Item.h"

#include <sstream>

#include "sync_random.h"

Item::Item(size_t id) : IMovable(id)
{
    passable_level = Passable::SMALL_ITEM;

    v_level = 5;
    SetSprite("icons/items.dmi");
}

Screwdriver::Screwdriver(size_t id) : Item(id)
{
    unsigned int value = get_rand() % 8;

    std::stringstream converter;
    converter << "screwdriver";
    if (value)
        converter << value;
    SetState(converter.str());
    
    name = "Screwdriver";
}

Wirecutters::Wirecutters(size_t id) : Item(id)
{
    SetState("cutters");

    name = "Wirecutters";
}

Crowbar::Crowbar(size_t id) : Item(id)
{
    SetState("crowbar");

    name = "Crowbar";
}

Wrench::Wrench(size_t id) : Item(id)
{
    SetState("wrench");

    name = "Wrench";
}
