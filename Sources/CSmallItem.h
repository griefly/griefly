#pragma once

#include "IMovable.h"

class SmallItem : public IMovable
{
public:
    DECLARE_SAVED(SmallItem, IMovable);
    DECLARE_GET_TYPE_ITEM(SmallItem);
    int KV_SAVEBLE(force);
    int KV_SAVEBLE(penet_force);
    virtual void use(){};
    SmallItem(size_t id) : IMovable(id)
    {
        force = 0;
        penet_force = 0;
        v_level = 7;
    }
};
ADD_TO_TYPELIST(SmallItem);

class Spear: public SmallItem
{
public:
    DECLARE_SAVED(Spear, SmallItem);
    DECLARE_GET_TYPE_ITEM(spear);
    Spear(size_t id) : SmallItem(id)
    {
        SetSprite("icons/spear.png");
        name = "Spear";
        penet_force = 1;
    }
};
ADD_TO_TYPELIST(Spear);

class ForceSpear: public SmallItem
{
public:
    DECLARE_SAVED(ForceSpear, SmallItem);
    DECLARE_GET_TYPE_ITEM(forcespear)
    ForceSpear(size_t id) : SmallItem(id)
    {
        SetSprite("icons/spear.png");
        name = "Forcespear";
//        imageStateW = 2;
        force = 1;
        SYSTEM_STREAM << "WARAFUCK I AM FORCESPEAR" << std::endl;
    }
};
ADD_TO_TYPELIST(ForceSpear);

class CMeat : public SmallItem
{
public:
    DECLARE_SAVED(CMeat, SmallItem);
    DECLARE_GET_TYPE_ITEM(meat)
    CMeat(size_t id);
    //bool coocked;
};
ADD_TO_TYPELIST(CMeat);

class CupItem : public SmallItem
{
public:
    DECLARE_SAVED(CupItem, SmallItem);
    DECLARE_GET_TYPE_ITEM(cupitem)
    CupItem(size_t id);
    virtual void use(){};
};
ADD_TO_TYPELIST(CupItem);

class CWeed : public SmallItem
{
public:
    DECLARE_SAVED(CWeed, SmallItem);
    DECLARE_GET_TYPE_ITEM(weed)
    virtual void process();
    CWeed(size_t id);
};
ADD_TO_TYPELIST(CWeed);
