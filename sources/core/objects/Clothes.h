#pragma once

#include "Item.h"

class Cloth: public Item
{
public:
    DECLARE_SAVED(Cloth, Item);
    DECLARE_GET_TYPE_ITEM(Cloth);
    Cloth(quint32 id);
};
ADD_TO_TYPELIST(Cloth);

class Armor: public Cloth
{
public:
    DECLARE_SAVED(Armor, Cloth);
    DECLARE_GET_TYPE_ITEM(Armor);
    Armor(quint32 id);
};
ADD_TO_TYPELIST(Armor);

class Helmet: public Cloth
{
public:
    DECLARE_SAVED(Helmet, Cloth);
    DECLARE_GET_TYPE_ITEM(Helmet);
    Helmet(quint32 id);
};
ADD_TO_TYPELIST(Helmet);

class Tophat: public Cloth
{
public:
    DECLARE_SAVED(Tophat, Cloth);
    DECLARE_GET_TYPE_ITEM(Tophat);
    Tophat(quint32 id);
};
ADD_TO_TYPELIST(Tophat);

class EngineUniform: public Cloth
{
public:
    DECLARE_SAVED(EngineUniform, Cloth);
    DECLARE_GET_TYPE_ITEM(EngineUniform);
    EngineUniform(quint32 id);
};
ADD_TO_TYPELIST(EngineUniform);

class JanitorUniform: public Cloth
{
public:
    DECLARE_SAVED(JanitorUniform, Cloth);
    DECLARE_GET_TYPE_ITEM(JanitorUniform);
    JanitorUniform(quint32 id);
};
ADD_TO_TYPELIST(JanitorUniform);

class ClownUniform: public Cloth
{
public:
    DECLARE_SAVED(ClownUniform, Cloth);
    DECLARE_GET_TYPE_ITEM(ClownUniform);
    ClownUniform(quint32 id);
};
ADD_TO_TYPELIST(ClownUniform);

class BarmanUniform: public Cloth
{
public:
    DECLARE_SAVED(BarmanUniform, Cloth);
    DECLARE_GET_TYPE_ITEM(BarmanUniform);
    BarmanUniform(quint32 id);
};
ADD_TO_TYPELIST(BarmanUniform);

class RedUniform: public Cloth
{
public:
    DECLARE_SAVED(RedUniform, Cloth);
    DECLARE_GET_TYPE_ITEM(RedUniform);
    RedUniform(quint32 id);
};
ADD_TO_TYPELIST(RedUniform);

class OrangeBoots: public Cloth
{
public:
    DECLARE_SAVED(OrangeBoots, Cloth);
    DECLARE_GET_TYPE_ITEM(OrangeBoots);
    OrangeBoots(quint32 id);
};
ADD_TO_TYPELIST(OrangeBoots);

class BlackBoots: public Cloth
{
public:
    DECLARE_SAVED(BlackBoots, Cloth);
    DECLARE_GET_TYPE_ITEM(BlackBoots);
    BlackBoots(quint32 id);
};
ADD_TO_TYPELIST(BlackBoots);

class ClownBoots: public Cloth
{
public:
    DECLARE_SAVED(ClownBoots, Cloth);
    DECLARE_GET_TYPE_ITEM(ClownBoots);
    ClownBoots(quint32 id);
};
ADD_TO_TYPELIST(ClownBoots);

class MedicalUniform: public Cloth
{
public:
    DECLARE_SAVED(MedicalUniform, Cloth);
    DECLARE_GET_TYPE_ITEM(MedicalUniform);
    MedicalUniform(quint32 id);
};
ADD_TO_TYPELIST(MedicalUniform);

class WhiteShoes: public Cloth
{
public:
    DECLARE_SAVED(WhiteShoes, Cloth);
    DECLARE_GET_TYPE_ITEM(WhiteShoes);
    WhiteShoes(quint32 id);
};
ADD_TO_TYPELIST(WhiteShoes);

