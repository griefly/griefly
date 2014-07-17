#include "Item.h"

#include <sstream>

#include "sync_random.h"

Screwdriver::Screwdriver()
{
    v_level = 5;

    SetSprite("icons/items.dmi");

    unsigned int value = get_rand() % 8;

    std::stringstream converter;
    converter << "screwdriver";
    if (value)
        converter << value;
    SetState(converter.str());
    
    name = "Screwdriver";
}