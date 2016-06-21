#pragma once

#include "Item.h"
#include <SFML/System/Vector2.hpp>
#include "Projectiles.h"
#include "Ammunationboxes.h"
#include "Projectiletypes.h"
#include "../Game.h"

class Revolver: public Item
{
public:
    DECLARE_SAVED(Revolver, Item);
    DECLARE_GET_TYPE_ITEM(Revolver);
    Revolver(size_t id);
    void Shoot(sf::Vector2i target);
 //   void Create_Projectile(sf::Vector2i& target);
 // TODO virtual shoot for every weapon
    int AddAmmunition();
    bool Handle_Ammo();   
    void AttackBy(id_ptr_on<Item> item);
private:
    int KV_SAVEBLE(ammunition_);
};
ADD_TO_TYPELIST(Revolver);
