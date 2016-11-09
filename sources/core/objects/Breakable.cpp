#include "Breakable.h"
#include "Mob.h"
#include "Projectiles.h"

Breakable::Breakable(quint32 id) : Structure(id)
{
   damage_points_ = 1;
}

void Breakable::AttackBy(IdPtr<Item> item)
{   
    if (!item.IsValid())
    {
        return;
    }
    if (IdPtr<Screwdriver> scr = item)
    {
        Structure::AttackBy(item);
        return;
    }
    damage_points_ -= item->damage;
    if (damage_points_ <= 0)
    {
        Break();
    }
}

void Breakable::Bump(IdPtr<IMovable> item)
{
    if (IdPtr<Projectile> projectile = item)
    {
        damage_points_ -= projectile->GetDamage();
    }
    if (damage_points_ <= 0)
    {
        Break();
        return;
    }
    IMovable::Bump(item);
}
