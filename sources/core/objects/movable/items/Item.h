#pragma once

#include "core/objects/movable/Movable.h"

namespace kv
{

class Item : public Movable
{
public:
    DECLARE_SAVEABLE(Item, Movable);
    REGISTER_CLASS_AS(Item);
    Item();
    QString KV_SAVEABLE(type);
    int KV_SAVEABLE(damage);
};
END_DECLARE(Item);

class Screwdriver : public Item
{
public:
    DECLARE_SAVEABLE(Screwdriver, Item);
    REGISTER_CLASS_AS(Screwdriver);
    Screwdriver();
    virtual void AfterWorldCreation() override;
};
END_DECLARE(Screwdriver);

class Wirecutters : public Item
{
public:
    DECLARE_SAVEABLE(Wirecutters, Item);
    REGISTER_CLASS_AS(Wirecutters);
    Wirecutters();
};
END_DECLARE(Wirecutters);

class Crowbar : public Item
{
public:
    DECLARE_SAVEABLE(Crowbar, Item);
    REGISTER_CLASS_AS(Crowbar);
    Crowbar();
};
END_DECLARE(Crowbar);

class Wrench : public Item
{
public:
    DECLARE_SAVEABLE(Wrench, Item);
    REGISTER_CLASS_AS(Wrench);
    Wrench();
};
END_DECLARE(Wrench);

class BulletCasing : public Item
{
public:
    DECLARE_SAVEABLE(BulletCasing, Item);
    REGISTER_CLASS_AS(BulletCasing);
    BulletCasing();
};
END_DECLARE(BulletCasing);

}
