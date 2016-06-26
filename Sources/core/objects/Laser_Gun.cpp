#include "Laser_Gun.h"
#include "Gun.cpp"

LaserGun::LaserGun(size_t id) : Gun(id)
{
    SetState("energykill100");

    name = "Laser Gun";
    
    ammunition_ = 20;
    max_ammunition_ = 20;
}

void LaserGun::Shoot(VDir target,id_ptr_on<Human> shooter)
{	
    if(Handle_Ammo())
    {
      if(GetOwner()->GetOwner().valid())
      {
          id_ptr_on<Projectile> p = Create<Projectile>(Laser::T_ITEM_S(),GetOwner()->GetOwner());
          p->MakeMovementPattern(target,shooter);
          unsigned int value = GetRand() % 2;
          std::string snd;
          if (value == 0)
              snd = "laser3.ogg";
          if (value == 1)
              snd = "Laser.ogg";
          PlaySoundIfVisible(snd, owner.ret_id());
      }
		//a counter that tells the number of bullets in magazine/clip
    }
    else
    {
          PlaySoundIfVisible("empty.ogg",owner.ret_id());
		//the *click* text from ss13
    }	
}
void LaserGun::AttackBy(id_ptr_on<Item> item)
{
    if (id_ptr_on<AmmunitionBox> r = item)
    {
	if(r->CheckBullets())
        {
		if(AddAmmunition())
                {
		    r->RemoveBullet();
	        }
	}
	else
	{
	// message which tells the player that the box is empty 
	}
    }
}
