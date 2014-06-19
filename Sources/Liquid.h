#pragma once

#include "DrawInt.h"
#include "GLSprite.h"

class Liquid: public IDraw
{
public:
    DECLARE_GET_TYPE_ITEM(liquid);
    DECLARE_SAVED(Liquid, IDraw);
    Liquid();

    //id_ptr owner;
    unsigned int KV_SAVEBLE(amount);
    bool KV_SAVEBLE(flammable);
    virtual void processImage(DrawType type);
    virtual void process();
    void SetSprite(const std::string& T_SPR);
    const GLSprite* GetSprite() { return sprite_;};
    int KV_SAVEBLE(imageStateH);
    int KV_SAVEBLE(imageStateW);
private:
    std::string KV_SAVEBLE(T_SPR);
    GLSprite* KV_ON_LOAD(sprite_, nullptr);
};
ADD_TO_TYPELIST(Liquid);

class LWater: public Liquid
{
public:
    DECLARE_GET_TYPE_ITEM(water);
    DECLARE_SAVED(LWater, Liquid);
    LWater()
    {
        SetSprite("icons/water.png");
    }
};
ADD_TO_TYPELIST(LWater);

class LPetrol: public Liquid
{
public:
    DECLARE_GET_TYPE_ITEM(petrol);
    DECLARE_SAVED(LPetrol, Liquid);
    LPetrol()
    {
        flammable = true;
        SetSprite("icons/petrol.png");
    }
};
ADD_TO_TYPELIST(LPetrol);

class LHoney: public Liquid
{
public:
    DECLARE_GET_TYPE_ITEM(honey);
    DECLARE_SAVED(LHoney, Liquid);
    LHoney()
    {
        SetSprite("icons/honey.png");
    }
};
ADD_TO_TYPELIST(LHoney);

class LFirehoney: public Liquid
{
public:
    DECLARE_GET_TYPE_ITEM(firehoney);
    DECLARE_SAVED(LFirehoney, Liquid);
    LFirehoney()
    {
        flammable = true;
        SetSprite("icons/firehoney.png");
    }
};
ADD_TO_TYPELIST(LFirehoney);

class LBlood: public Liquid
{
public:
    DECLARE_GET_TYPE_ITEM(liquidblood);
    DECLARE_SAVED(LBlood, Liquid);
    LBlood()
    {
        SetSprite("icons/blood.png");
    }
};
ADD_TO_TYPELIST(LBlood);