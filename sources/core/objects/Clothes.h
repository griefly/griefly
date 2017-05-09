#pragma once

#include "Item.h"

namespace kv
{

class Cloth : public Item
{
public:
    DECLARE_SAVED(Cloth, Item);
    DECLARE_GET_TYPE_ITEM(Cloth);
    Cloth();
};
ADD_TO_TYPELIST(Cloth);

class Armor : public Cloth
{
public:
    DECLARE_SAVED(Armor, Cloth);
    DECLARE_GET_TYPE_ITEM(Armor);
    Armor();
};
ADD_TO_TYPELIST(Armor);

class Helmet : public Cloth
{
public:
    DECLARE_SAVED(Helmet, Cloth);
    DECLARE_GET_TYPE_ITEM(Helmet);
    Helmet();
};
ADD_TO_TYPELIST(Helmet);

class Tophat : public Cloth
{
public:
    DECLARE_SAVED(Tophat, Cloth);
    DECLARE_GET_TYPE_ITEM(Tophat);
    Tophat();
};
ADD_TO_TYPELIST(Tophat);

class EngineUniform : public Cloth
{
public:
    DECLARE_SAVED(EngineUniform, Cloth);
    DECLARE_GET_TYPE_ITEM(EngineUniform);
    EngineUniform();
};
ADD_TO_TYPELIST(EngineUniform);

class JanitorUniform : public Cloth
{
public:
    DECLARE_SAVED(JanitorUniform, Cloth);
    DECLARE_GET_TYPE_ITEM(JanitorUniform);
    JanitorUniform();
};
ADD_TO_TYPELIST(JanitorUniform);

class ClownUniform : public Cloth
{
public:
    DECLARE_SAVED(ClownUniform, Cloth);
    DECLARE_GET_TYPE_ITEM(ClownUniform);
    ClownUniform();
};
ADD_TO_TYPELIST(ClownUniform);

class BarmanUniform : public Cloth
{
public:
    DECLARE_SAVED(BarmanUniform, Cloth);
    DECLARE_GET_TYPE_ITEM(BarmanUniform);
    BarmanUniform();
};
ADD_TO_TYPELIST(BarmanUniform);

class RedUniform : public Cloth
{
public:
    DECLARE_SAVED(RedUniform, Cloth);
    DECLARE_GET_TYPE_ITEM(RedUniform);
    RedUniform();
};
ADD_TO_TYPELIST(RedUniform);

class OrangeBoots : public Cloth
{
public:
    DECLARE_SAVED(OrangeBoots, Cloth);
    DECLARE_GET_TYPE_ITEM(OrangeBoots);
    OrangeBoots();
};
ADD_TO_TYPELIST(OrangeBoots);

class BlackBoots : public Cloth
{
public:
    DECLARE_SAVED(BlackBoots, Cloth);
    DECLARE_GET_TYPE_ITEM(BlackBoots);
    BlackBoots();
};
ADD_TO_TYPELIST(BlackBoots);

class ClownBoots : public Cloth
{
public:
    DECLARE_SAVED(ClownBoots, Cloth);
    DECLARE_GET_TYPE_ITEM(ClownBoots);
    ClownBoots();
};
ADD_TO_TYPELIST(ClownBoots);

class MedicalUniform : public Cloth
{
public:
    DECLARE_SAVED(MedicalUniform, Cloth);
    DECLARE_GET_TYPE_ITEM(MedicalUniform);
    MedicalUniform();
};
ADD_TO_TYPELIST(MedicalUniform);

class WhiteShoes : public Cloth
{
public:
    DECLARE_SAVED(WhiteShoes, Cloth);
    DECLARE_GET_TYPE_ITEM(WhiteShoes);
    WhiteShoes();
};
ADD_TO_TYPELIST(WhiteShoes);

}
