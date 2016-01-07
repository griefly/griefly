#pragma once

#include "Item.h"

class FloorTile: public Item
{
public:
    DECLARE_SAVED(FloorTile, Item);
    DECLARE_GET_TYPE_ITEM(FloorTile);
    FloorTile(size_t id);
};
ADD_TO_TYPELIST(FloorTile);
