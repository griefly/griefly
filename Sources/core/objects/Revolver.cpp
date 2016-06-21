#include "Revolver.h"

Revolver::Revolver(size_t id) : Item(id)
{
    SetSprite("icons/guns.dmi");
    SetState("revolver");

    name = "Revolver",
    
    ammunition_ = 6;
}

bool Revolver::Handle_Ammo()
{
    if (ammunition_ <= 0)
    {
        ammunition_ = 0;
        return false;
    }
    --ammunition_;
    return true;
}

void Revolver::Shoot(sf::Vector2i target)
{	  
    if(Handle_Ammo())
    {
        id_ptr_on<Projectile> p = Create<Projectile>(Bullet::T_ITEM_S(),GetOwner());
        p->MakeMovementPattern(target);
        Game* a  = static_cast<Game*>(GetGame().GetTrueGame());
        a->GetProjectileProcess().insert(std::pair<int,id_ptr_on<Projectile>>(p.ret_id(),p));
             //       Create<Item>(Bullet_Casing::T_ITEM_S(),GetOwner);	
		//sound and a counter that tells the number of bullets in magazine/clip
    }
    else
    {
		//sound and the *click* text from ss13
    }	
}

int Revolver::AddAmmunition()
{
    if (ammunition_ < 0 && ammunition_ > 6)
    {
        ++ammunition_;
	return ammunition_;
    }    
    if (ammunition_ > 6)
    {
	--ammunition_;
        return ammunition_;
    }
}
void Revolver::AttackBy(id_ptr_on<Item> item)
{
    if (id_ptr_on<AmmunitionBox> r = item)
    {
	if(r->CheckBullets())
        {
		r->RemoveBullet();
	}
	else
	{
	// message which tells the player that the box is empty 
	}
    }
}
