#include "Revolver.h"
#include "Gun.cpp"

Revolver::Revolver(size_t id) : Gun(id)
{
    SetState("revolver");

    name = "Revolver";
    
    ammunition_ = 6;
    max_ammunition_ = 6;
}

void Revolver::Shoot(VDir target,id_ptr_on<Human> shooter)
{	
    if(Handle_Ammo())
    {
      if(GetOwner()->GetOwner().valid())
      {
          id_ptr_on<Projectile> p = Create<Projectile>(Bullet::T_ITEM_S(),GetOwner()->GetOwner());
          p->MakeMovementPattern(target,shooter);
          std::string snd;
          snd = "Gunshot.ogg";
          PlaySoundIfVisible(snd, owner.ret_id());
          id_ptr_on<Item> bc =Create<Item>(BulletCasing::T_ITEM_S(),GetOwner()->GetOwner());
          unsigned int r = GetRand() % 4;
          switch(r)
          {
          case(0):
              bc->Rotate(D_RIGHT);
              break;
          case(1):
              bc->Rotate(D_LEFT);
              break;
          case(3):
              bc->Rotate(D_DOWN);
              break;
          case(4):
              bc->Rotate(D_UP);
              break;
          }
      }	
		//a counter that tells the number of bullets in magazine/clip
    }
    else
    {
        PlaySoundIfVisible("empty.ogg",owner.ret_id());
		//sound and the *click* text from ss13
    }	
}
void Revolver::AttackBy(id_ptr_on<Item> item)
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
