#pragma once

#include "Item.h"
#include "core/objects/mobs/Human.h"

namespace kv
{

class Drinks : public Item
{
public:
    DECLARE_SAVEABLE(Drinks, Item);
    REGISTER_CLASS_AS(Drinks);
    Drinks();
    void Drink(IdPtr<Human> consumer, IdPtr<Human> feeder);
private:
    bool KV_SAVEABLE(full_);
};
END_DECLARE(Drinks);

class Vodka : public Drinks
{
public:
    DECLARE_SAVEABLE(Vodka, Drinks);
    REGISTER_CLASS_AS(Vodka);
    Vodka();
};
END_DECLARE(Vodka);

class Beer : public Drinks
{
public:
    DECLARE_SAVEABLE(Beer, Drinks);
    REGISTER_CLASS_AS(Beer);
    Beer();
};
END_DECLARE(Beer);

class Tea : public Drinks
{
public:
    DECLARE_SAVEABLE(Tea, Drinks);
    REGISTER_CLASS_AS(Tea);
    Tea();
};
END_DECLARE(Tea);

}
