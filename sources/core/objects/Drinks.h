#pragma once

#include "Item.h"
#include "Human.h"

class Drinks: public Item
{
public:
    DECLARE_SAVED(Drinks, Item);
    DECLARE_GET_TYPE_ITEM(Drinks);
    Drinks();
    void Drink(IdPtr<Human> human1, IdPtr<Human> human2);
private:
    bool KV_SAVEBLE(full_);
};
ADD_TO_TYPELIST(Drinks);

class Vodka: public Drinks
{
public:
    DECLARE_SAVED(Vodka, Drinks);
    DECLARE_GET_TYPE_ITEM(Vodka);
    Vodka();
};
ADD_TO_TYPELIST(Vodka);

class Beer: public Drinks
{
public:
    DECLARE_SAVED(Beer, Drinks);
    DECLARE_GET_TYPE_ITEM(Beer);
    Beer();
};
ADD_TO_TYPELIST(Beer);

class Tea: public Drinks
{
public:
    DECLARE_SAVED(Tea, Drinks);
    DECLARE_GET_TYPE_ITEM(Tea);
    Tea();
};
ADD_TO_TYPELIST(Tea);

