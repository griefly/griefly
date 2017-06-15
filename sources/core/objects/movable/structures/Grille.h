#pragma once

#include "Breakable.h"

namespace kv
{

class Grille : public Breakable
{
public:
    DECLARE_SAVEABLE(Grille, Breakable);
    REGISTER_CLASS_AS(Grille);
    Grille();
    virtual void AttackBy(IdPtr<Item> item) override;
    virtual void Break() override;
    virtual void PlayOnHitSound() override;
private:
    bool KV_SAVEABLE(cutted_);
};
END_DECLARE(Grille);

}
