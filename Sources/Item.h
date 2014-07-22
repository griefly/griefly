#pragma once

#include "IMovable.h"

class Item: public IMovable
{
public:
    DECLARE_SAVED(Item, IMovable);
    DECLARE_GET_TYPE_ITEM(Item);
    Item();
};
ADD_TO_TYPELIST(Item);

class Screwdriver: public Item
{
public:
    DECLARE_SAVED(Screwdriver, Item);
    DECLARE_GET_TYPE_ITEM(Screwdriver);
    Screwdriver();
};
ADD_TO_TYPELIST(Screwdriver);

class Wirecutters: public Item
{
public:
    DECLARE_SAVED(Wirecutters, Item);
    DECLARE_GET_TYPE_ITEM(Wirecutters);
    Wirecutters();
};
ADD_TO_TYPELIST(Wirecutters);

class Crowbar: public Item
{
public:
    DECLARE_SAVED(Crowbar, Item);
    DECLARE_GET_TYPE_ITEM(Crowbar);
    Crowbar();
};
ADD_TO_TYPELIST(Crowbar);