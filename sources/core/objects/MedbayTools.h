#pragma once

#include "Item.h"
#include "Human.h"

class HealthAnalyzer: public Item
{
public:
    DECLARE_SAVED(HealthAnalyzer, Item);
    DECLARE_GET_TYPE_ITEM(HealthAnalyzer);
    HealthAnalyzer(quint32 id);
    void Scan(IdPtr<Human> target);
};
ADD_TO_TYPELIST(HealthAnalyzer);

class Medicine: public Item
{
public:
    DECLARE_SAVED(Medicine, Item);
    DECLARE_GET_TYPE_ITEM(Medicine);
    Medicine(quint32 id);
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
    Ointment(quint32 id);
};
ADD_TO_TYPELIST(Ointment);

class BrutePatch: public Medicine
{
public:
    DECLARE_SAVED(BrutePatch, Medicine);
    DECLARE_GET_TYPE_ITEM(BrutePatch);
    BrutePatch(quint32 id);
};
ADD_TO_TYPELIST(BrutePatch);
