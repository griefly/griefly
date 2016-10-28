#include "ProjectileTypes.h"

Bullet::Bullet(size_t id) : Projectile(id)
{
    damage_ = 20;
    SetState("bullet");
    name = "Bullet";
}

Laser::Laser(size_t id) : Projectile(id)
{
    damage_ = 10;
    SetState("laser");
    name = "Laser";
}

bool Laser::CheckPassable()
{
    if (CheckHumanTile())
    {
        return false;
    }
    auto tile = GetOwner();
    if (tile->IsTransparent())
    {
        auto nearby = tile->GetNeighbour(GetDir());
        if (nearby->IsTransparent())
        {
            return true;
        }
    }
    if (IMovable::CheckPassable())
    {
        return true;
    }
    Delete();
    return false;
}
