#include "Projectiletypes.h"

Bullet::Bullet(size_t id) : Projectile(id)
{
    damage = 5;
    SetState("laser");
    name = "Bullet";
}
