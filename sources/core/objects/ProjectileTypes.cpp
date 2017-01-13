#include "ProjectileTypes.h"

Bullet::Bullet(quint32 id) : Projectile(id)
{
    damage_ = 20;
    SetState("bullet");
    name = "Bullet";
}

Laser::Laser(quint32 id) : Projectile(id)
{
    burn_damage_ = 10;
    SetState("laser");
    name = "Laser";
}

bool Laser::CheckPassable()
{
    if (!harmless_)
    {
        if (CheckHumanTile())
        {
            return false;
        }
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

    // TODO (?): Bump() by Laser.
    // We cannot call IMovable::CheckPassable here because then
    // laser projectiles will bump glasses, so it is needed to add bump here

    Delete();
    return false;
}
