#pragma once

#include "Item.h"

class FloorTile: public Item
{
public:
    DECLARE_SAVED(FloorTile, Item);
    DECLARE_GET_TYPE_ITEM(FloorTile);
    FloorTile(quint32 id);
    QString KV_SAVEBLE(Type);
};
ADD_TO_TYPELIST(FloorTile);

class WhiteFloorTile: public FloorTile
{
public:
    DECLARE_SAVED(WhiteFloorTile, FloorTile);
    DECLARE_GET_TYPE_ITEM(WhiteFloorTile);
    WhiteFloorTile(quint32 id);
};
ADD_TO_TYPELIST(WhiteFloorTile);
