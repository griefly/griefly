#pragma once

#include "Item.h"

namespace kv
{

class Rod : public Item
{
public:
    DECLARE_SAVEABLE(Rod, Item);
    REGISTER_CLASS_AS(Rod);
    Rod();
    virtual void AttackBy(IdPtr<Item> item) override;
};
END_DECLARE(Rod);

class Metal : public Item
{
public:
    DECLARE_SAVEABLE(Metal, Item);
    REGISTER_CLASS_AS(Metal);
    Metal();
    virtual void AttackBy(IdPtr<Item> item) override;
};
END_DECLARE(Metal);

}
