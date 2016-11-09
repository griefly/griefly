#pragma once

#include "Structure.h"

class Breakable : public Structure
{
public:
    DECLARE_SAVED(Breakable, Structure);
    DECLARE_GET_TYPE_ITEM(Breakable);
    Breakable(quint32 id);

    virtual void AttackBy(IdPtr<Item> item) override;
    virtual void Bump(IdPtr<IMovable> item) override;
    virtual void Break() {};
private:
    int KV_SAVEBLE(damage_points_);
};
ADD_TO_TYPELIST(Breakable);
