#pragma once

#include "Item.h"

namespace kv
{

class FloorTile : public Item
{
public:
    DECLARE_SAVEABLE(FloorTile, Item);
    REGISTER_CLASS_AS(FloorTile);
    FloorTile();
};
END_DECLARE(FloorTile);

}
