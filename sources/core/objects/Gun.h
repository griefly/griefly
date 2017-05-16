#pragma once

#include "Item.h"
#include "Projectiles.h"
#include "AmmunitionBoxes.h"
#include "ProjectileTypes.h"
#include "../Game.h"

namespace kv
{

class Gun : public Item
{
public:
    DECLARE_SAVEABLE(Gun, Item);
    REGISTER_CLASS_AS(Gun);
    Gun();
    virtual void Shoot(VDir target);
    bool AddAmmo();
    bool UseAmmo();
    virtual void AttackBy(IdPtr<Item> item);
    static bool Targetable(IdPtr<MapObject> item);
    VDir TargetTileLoc(IdPtr<MapObject> item) const;
protected:
    int KV_SAVEABLE(max_ammunition_);
    int KV_SAVEABLE(ammunition_);
    void ShootImpl(VDir target, const QString& sound,
                   const QString& projectile_type, const QString& casing_type);
};
END_DECLARE(Gun);

}
