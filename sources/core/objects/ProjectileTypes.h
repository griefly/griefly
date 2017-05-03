#pragma once

#include "Projectiles.h"

class Bullet: public Projectile
{
public:
    DECLARE_SAVED(Bullet, Projectile);
    DECLARE_GET_TYPE_ITEM(Bullet);
    Bullet();
};
ADD_TO_TYPELIST(Bullet);

class Laser: public Projectile
{
public:
    DECLARE_SAVED(Laser, Projectile);
    DECLARE_GET_TYPE_ITEM(Laser);
    Laser();

    virtual bool CheckPassable() override;
};
ADD_TO_TYPELIST(Laser);

