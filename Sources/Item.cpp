#include "Item.h"

#include <sstream>

#include "sync_random.h"

Item::Item()
{
    v_level = 5;
    SetSprite("icons/items.dmi");
}

Screwdriver::Screwdriver()
{
    unsigned int value = get_rand() % 8;

    std::stringstream converter;
    converter << "screwdriver";
    if (value)
        converter << value;
    SetState(converter.str());
    
    name = "Screwdriver";
}

Wirecutters::Wirecutters()
{
    SetState("cutters");

    name = "Wirecutters";
}