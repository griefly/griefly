#pragma once

#include "Movable.h"

class Item: public IMovable
{
public:
    DECLARE_SAVED(Item, IMovable);
    DECLARE_GET_TYPE_ITEM(Item);
    Item();
    QString KV_SAVEBLE(type);
    int KV_SAVEBLE(damage);
};
ADD_TO_TYPELIST(Item);

class Screwdriver: public Item
{
public:
    DECLARE_SAVED(Screwdriver, Item);
    DECLARE_GET_TYPE_ITEM(Screwdriver);
    Screwdriver();
    virtual void AfterWorldCreation() override;
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

class Wrench: public Item
{
public:
    DECLARE_SAVED(Wrench, Item);
    DECLARE_GET_TYPE_ITEM(Wrench);
    Wrench();
};
ADD_TO_TYPELIST(Wrench);

class BulletCasing: public Item
{
public:
    DECLARE_SAVED(BulletCasing, Item);
    DECLARE_GET_TYPE_ITEM(BulletCasing);
    BulletCasing();
};
ADD_TO_TYPELIST(BulletCasing);
