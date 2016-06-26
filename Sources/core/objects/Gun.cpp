#include "Gun.h"

Gun::Gun(size_t id) : Item(id)
{
    SetSprite("icons/guns.dmi");

}

bool Gun::Handle_Ammo()
{
    if (ammunition_ <= 0)
    {
        ammunition_ = 0;
        return false;
    }
    --ammunition_;
    return true;
}

void Gun::Shoot(VDir target,id_ptr_on<Human> shooter)
{	
    if(Handle_Ammo())
    {
      if(GetOwner()->GetOwner().valid())
      {
          id_ptr_on<Projectile> p = Create<Projectile>(Bullet::T_ITEM_S(),GetOwner()->GetOwner());
          p->MakeMovementPattern(target,shooter);
      }
             //       Create<Item>(Bullet_Casing::T_ITEM_S(),GetOwner);	
    }
    else
    {
          PlaySoundIfVisible("empty.ogg",owner.ret_id());
		// the *click* text from ss13
    }	
}

int Gun::AddAmmunition()
{
    if (ammunition_ < max_ammunition_)
    {
        ++ammunition_;
	return ammunition_;
    }    
    if (ammunition_ > max_ammunition_)
    {
	--ammunition_;
        return ammunition_;
    }
}
void Gun::AttackBy(id_ptr_on<Item> item)
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
