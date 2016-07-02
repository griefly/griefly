#include "Weapons.h"
#include "representation/Sound.h"
#include "Tile.h"


LaserGun::LaserGun(size_t id) : Gun(id)
{
    SetState("energykill100");

    name = "Laser Gun";
    
    ammunition_ = 20;
    max_ammunition_ = 20;
}

void LaserGun::Shoot(VDir target)
{
    unsigned int value = GetRand() % 2;
    std::string snd;
    if (value == 0)
    {
        snd = "laser3.ogg";
    }
    if (value == 1)
    {
        snd = "Laser.ogg";
    }
    ShootImpl(target,snd,Laser::T_ITEM_S(),"");
}
void LaserGun::AttackBy(id_ptr_on<Item> item)
{
    if (id_ptr_on<AmmunitionBox> r = item)
    {
        if(r->CheckBullets())
        {
            if(AddAmmo())
            {
                r->RemoveBullet();
                return;
            }
        }
        // message which tells the player that the box is empty 
    }
}


Revolver::Revolver(size_t id) : Gun(id)
{
    SetState("revolver");

    name = "Revolver";
    
    ammunition_ = 6;
    max_ammunition_ = 6;
}

void Revolver::Shoot(VDir target)
{
    std::string snd;
    snd = "Gunshot.ogg";
    ShootImpl(target,snd,Bullet::T_ITEM_S(),BulletCasing::T_ITEM_S());
}
void Revolver::AttackBy(id_ptr_on<Item> item)
{
    if (id_ptr_on<AmmunitionBox> r = item)
    {
        if(r->CheckBullets())
        {
		    if(AddAmmo())
            {
                r->RemoveBullet();
                return;
            }
	    }
	    // message which tells the player that the box is empty 
    }
}
