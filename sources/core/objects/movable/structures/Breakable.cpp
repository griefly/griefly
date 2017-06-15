#include "Breakable.h"
#include "core/objects/mobs/Mob.h"
#include "core/objects/movable/Projectiles.h"

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
    Hit(item->damage);
}

void Breakable::Bump(IdPtr<Movable> item)
{
    if (IdPtr<Projectile> projectile = item)
    {
        Hit(projectile->GetDamage());
        return;
    }
    Movable::Bump(item);
}

void Breakable::SetHitPoints(int number)
{
    hit_points_ = number;
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
