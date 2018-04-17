#include "Breakable.h"

#include "objects/mobs/Mob.h"
#include "objects/movable/Projectiles.h"
#include "objects/movable/items/Item.h"

using namespace kv;

Breakable::Breakable()
{
    hit_points_ = 1;
}

void Breakable::AttackBy(IdPtr<Item> item)
{   
    if (!item.IsValid())
    {
        return;
    }
    if (IdPtr<Screwdriver> driver = item)
    {
        Structure::AttackBy(item);
        return;
    }
    Hit(item->GetDamage());
}

void Breakable::Bump(const Vector& vector, IdPtr<Movable> item)
{
    if (IdPtr<Projectile> projectile = item)
    {
        Hit(projectile->GetDamage());
        return;
    }
    Movable::Bump(vector, item);
}

void Breakable::SetHitPoints(int hit_points)
{
    hit_points_ = hit_points;
}

void Breakable::Hit(int damage)
{
    hit_points_ -= damage;
    PlayOnHitSound();
    if (hit_points_ <= 0)
    {
        Break();
    }
}
