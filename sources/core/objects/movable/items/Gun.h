#pragma once

#include "Item.h"
#include "AmmunitionBoxes.h"

#include "objects/movable/ProjectileTypes.h"
#include "objects/movable/Projectiles.h"

namespace kv
{

class Gun : public Item
{
public:
    DECLARE_SAVEABLE(Gun, Item);
    REGISTER_CLASS_AS(Gun);
    Gun();
    virtual void Shoot(Vector target);
    bool AddAmmo();
    bool UseAmmo();
    virtual void AttackBy(IdPtr<Item> item);
    static bool Targetable(IdPtr<MapObject> item);
    Vector TargetTileLoc(IdPtr<MapObject> item) const;
protected:
    int KV_SAVEABLE(max_ammunition_);
    int KV_SAVEABLE(ammunition_);
    void ShootImpl(Vector target, const QString& sound,
                   const QString& projectile_type, const QString& casing_type);
};
END_DECLARE(Gun);

}
