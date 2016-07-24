#pragma once

#include "Item.h"
#include "Projectiles.h"
#include "AmmunitionBoxes.h"
#include "Projectiletypes.h"
#include "../Game.h"

class Gun: public Item
{
public:
    DECLARE_SAVED(Gun, Item);
    DECLARE_GET_TYPE_ITEM(Gun);
    Gun(size_t id);
    virtual void Shoot(VDir target);
    bool AddAmmo();
    bool UseAmmo();   
    virtual void AttackBy(id_ptr_on<Item> item);
    static bool Targetable(id_ptr_on<IOnMapBase> item);
    VDir TargetTileLoc(id_ptr_on<IOnMapBase> item) const;
protected:
    int KV_SAVEBLE(max_ammunition_);
    int KV_SAVEBLE(ammunition_);
    void ShootImpl(VDir target, const std::string& sound,
                   const std::string& projectile_type, const std::string& casing_type);
};
ADD_TO_TYPELIST(Gun);
