#pragma once 

#include "Item.h"

class Shard: public Item
{
public:
    DECLARE_SAVED(Shard, Item);
    DECLARE_GET_TYPE_ITEM(Shard);
    Shard(size_t id);
};
ADD_TO_TYPELIST(Shard);
