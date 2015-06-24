#pragma once

#include "IMovable.h"

class Item: public IMovable
{
public:
    DECLARE_SAVED(Item, IMovable);
    DECLARE_GET_TYPE_ITEM(Item);
    Item(size_t id);
    std::string KV_SAVEBLE(type);
};
ADD_TO_TYPELIST(Item);

class Screwdriver: public Item
{
public:
    DECLARE_SAVED(Screwdriver, Item);
    DECLARE_GET_TYPE_ITEM(Screwdriver);
    Screwdriver(size_t id);
};
ADD_TO_TYPELIST(Screwdriver);

class Wirecutters: public Item
{
public:
    DECLARE_SAVED(Wirecutters, Item);
    DECLARE_GET_TYPE_ITEM(Wirecutters);
    Wirecutters(size_t id);
};
ADD_TO_TYPELIST(Wirecutters);

class Crowbar: public Item
{
public:
    DECLARE_SAVED(Crowbar, Item);
    DECLARE_GET_TYPE_ITEM(Crowbar);
    Crowbar(size_t id);
};
ADD_TO_TYPELIST(Crowbar);

class Wrench: public Item
{
public:
    DECLARE_SAVED(Wrench, Item);
    DECLARE_GET_TYPE_ITEM(Wrench);
    Wrench(size_t id);
};
ADD_TO_TYPELIST(Wrench);

class SimpleCloth: public Item
{
public:
    DECLARE_SAVED(SimpleCloth, Item);
    DECLARE_GET_TYPE_ITEM(SimpleCloth);
    SimpleCloth(size_t id);
};
ADD_TO_TYPELIST(SimpleCloth);

