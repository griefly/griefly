#include "Breakable.h"
#include "Mob.h"
#include "Projectiles.h"

Breakable::Breakable(quint32 id) : Structure(id)
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
    hit_points_ -= item->damage;
    PlayOnHitSound();
    if (hit_points_ <= 0)
    {
        Break();
    }
}

void Breakable::Bump(IdPtr<IMovable> item)
{
    if (IdPtr<Projectile> projectile = item)
    {
        hit_points_ -= projectile->GetDamage();
        PlayOnHitSound();
    }
    if (hit_points_ <= 0)
    {
        Break();
        return;
    }
    IMovable::Bump(item);
}

void Breakable::AddHitPoints(int number)
{
    hit_points_ += number;
}
