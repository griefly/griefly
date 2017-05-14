#pragma once

#include "Projectiles.h"

namespace kv
{

class Bullet : public Projectile
{
public:
    DECLARE_SAVEABLE(Bullet, Projectile);
    REGISTER_CLASS_AS(Bullet);
    Bullet();
};
END_DECLARE(Bullet);

class Laser : public Projectile
{
public:
    DECLARE_SAVEABLE(Laser, Projectile);
    REGISTER_CLASS_AS(Laser);
    Laser();

    virtual bool CheckPassable() override;
};
END_DECLARE(Laser);

}
