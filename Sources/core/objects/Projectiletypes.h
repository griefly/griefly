#pragma once

#include "Projectiles.h"

class Bullet: public Projectile
{
public:
    DECLARE_SAVED(Bullet, Projectile);
    DECLARE_GET_TYPE_ITEM(Bullet);
    Bullet(size_t id);
};
ADD_TO_TYPELIST(Bullet);

