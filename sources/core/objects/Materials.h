#pragma once

#include "Item.h"

namespace kv
{

class Rod : public Item
{
public:
    DECLARE_SAVED(Rod, Item);
    DECLARE_GET_TYPE_ITEM(Rod);
    Rod();
    virtual void AttackBy(IdPtr<Item> item) override;
};
ADD_TO_TYPELIST(Rod);

class Metal : public Item
{
public:
    DECLARE_SAVED(Metal, Item);
    DECLARE_GET_TYPE_ITEM(Metal);
    Metal();
    virtual void AttackBy(IdPtr<Item> item) override;
};
ADD_TO_TYPELIST(Metal);

}
