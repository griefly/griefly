#pragma once

#include "Item.h"

class Cloth: public Item
{
public:
    DECLARE_SAVED(Cloth, Item);
    DECLARE_GET_TYPE_ITEM(Cloth);
    Cloth(size_t id);
};
ADD_TO_TYPELIST(Cloth);

class Armor: public Cloth
{
public:
    DECLARE_SAVED(Armor, Cloth);
    DECLARE_GET_TYPE_ITEM(Armor);
    Armor(size_t id);
};
ADD_TO_TYPELIST(Armor);

class Helmet: public Cloth
{
public:
    DECLARE_SAVED(Helmet, Cloth);
    DECLARE_GET_TYPE_ITEM(Helmet);
    Helmet(size_t id);
};
ADD_TO_TYPELIST(Helmet);

class EngineUniform: public Cloth
{
public:
    DECLARE_SAVED(EngineUniform, Cloth);
    DECLARE_GET_TYPE_ITEM(EngineUniform);
    EngineUniform(size_t id);
};
ADD_TO_TYPELIST(EngineUniform);

class OrangeBoots: public Cloth
{
public:
    DECLARE_SAVED(OrangeBoots, Cloth);
    DECLARE_GET_TYPE_ITEM(OrangeBoots);
    OrangeBoots(size_t id);
};
ADD_TO_TYPELIST(OrangeBoots);



