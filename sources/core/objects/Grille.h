#pragma once

#include "Breakable.h"

namespace kv
{

class Grille : public Breakable
{
public:
    DECLARE_SAVED(Grille, Breakable);
    DECLARE_GET_TYPE_ITEM(Grille);
    Grille();
    virtual void AttackBy(IdPtr<Item> item) override;
    virtual void Break() override;
    virtual void PlayOnHitSound() override;
private:
    bool KV_SAVEBLE(cutted_);
};
ADD_TO_TYPELIST(Grille);

}
