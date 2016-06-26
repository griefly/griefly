#pragma once

#include "Item.h"
#include "Projectiles.h"
#include "Ammunationboxes.h"
#include "Projectiletypes.h"
#include "../Game.h"

class Gun: public Item
{
public:
    DECLARE_SAVED(Gun, Item);
    DECLARE_GET_TYPE_ITEM(Gun);
    Gun(size_t id);
    virtual void Shoot(VDir target,id_ptr_on<Human> shooter);
    int AddAmmunition();
    bool Handle_Ammo();   
    virtual void AttackBy(id_ptr_on<Item> item);
protected:
    int KV_SAVEBLE(max_ammunition_);
    int KV_SAVEBLE(ammunition_);
};
ADD_TO_TYPELIST(Gun);
