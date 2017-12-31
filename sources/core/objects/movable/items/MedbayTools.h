#pragma once

#include "Item.h"
#include "objects/mobs/Human.h"

namespace kv
{

class HealthAnalyzer : public Item
{
public:
    DECLARE_SAVEABLE(HealthAnalyzer, Item);
    REGISTER_CLASS_AS(HealthAnalyzer);
    HealthAnalyzer();
    void Scan(IdPtr<Human> target);
};
END_DECLARE(HealthAnalyzer);

class Medicine : public Item
{
public:
    DECLARE_SAVEABLE(Medicine, Item);
    REGISTER_CLASS_AS(Medicine);
    Medicine();
    void Heal(IdPtr<Human> target);
protected:
    int KV_SAVEABLE(burn_heal_);
    int KV_SAVEABLE(brute_heal_);
};
END_DECLARE(Medicine);

class Ointment : public Medicine
{
public:
    DECLARE_SAVEABLE(Ointment, Medicine);
    REGISTER_CLASS_AS(Ointment);
    Ointment();
};
END_DECLARE(Ointment);

class BruisePack : public Medicine
{
public:
    DECLARE_SAVEABLE(BruisePack, Medicine);
    REGISTER_CLASS_AS(BruisePack);
    BruisePack();
};
END_DECLARE(BruisePack);

}
