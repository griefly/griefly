#pragma once

#include "Item.h"
#include "Human.h"

class HealthAnalyzer: public Item
{
public:
    DECLARE_SAVED(HealthAnalyzer, Item);
    DECLARE_GET_TYPE_ITEM(HealthAnalyzer);
    HealthAnalyzer();
    void Scan(IdPtr<Human> target);
};
ADD_TO_TYPELIST(HealthAnalyzer);

class Medicine: public Item
{
public:
    DECLARE_SAVED(Medicine, Item);
    DECLARE_GET_TYPE_ITEM(Medicine);
    Medicine();
    void Heal(IdPtr<Human> target);
protected:
    int KV_SAVEBLE(burn_heal_);
    int KV_SAVEBLE(brute_heal_);
};
ADD_TO_TYPELIST(Medicine);

class Ointment: public Medicine
{
public:
    DECLARE_SAVED(Ointment, Medicine);
    DECLARE_GET_TYPE_ITEM(Ointment);
    Ointment();
};
ADD_TO_TYPELIST(Ointment);

class BruisePack: public Medicine
{
public:
    DECLARE_SAVED(BruisePack, Medicine);
    DECLARE_GET_TYPE_ITEM(BruisePack);
    BruisePack();
};
ADD_TO_TYPELIST(BruisePack);
