#pragma once 

#include "Item.h"

namespace kv
{

class Shard : public Item
{
public:
    DECLARE_SAVED(Shard, Item);
    DECLARE_GET_TYPE_ITEM(Shard);
    Shard();
    virtual void AfterWorldCreation() override;
};
ADD_TO_TYPELIST(Shard);

}
