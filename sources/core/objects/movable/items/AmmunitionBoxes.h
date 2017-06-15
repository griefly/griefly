#pragma once

#include "Item.h"

namespace kv
{

class AmmunitionBox : public Item
{
public:
    DECLARE_SAVEABLE(AmmunitionBox, Item);
    REGISTER_CLASS_AS(AmmunitionBox);
    AmmunitionBox();
    void RemoveBullet();
    bool CheckBullets();
private:
    int KV_SAVEABLE(bullets_);
};
END_DECLARE(AmmunitionBox);

}
