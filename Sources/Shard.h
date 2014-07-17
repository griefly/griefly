#pragma once 

#include "Item.h"

class Shard: public Item
{
public:
    DECLARE_SAVED(Shard, Item);
    DECLARE_GET_TYPE_ITEM(Shard);
    Shard();
};
ADD_TO_TYPELIST(Shard);