#include "Projectiles.h"
#include "../Helpers.h"


Projectile::Projectile(size_t id) : IMovable(id)
{
    SetPassable(D_ALL, Passable::AIR);
    tickSpeed = 0.25;
    damage = 0;
    SetSprite("icons/projectiles.dmi");
    v_level = 5;
}
void Projectile::Process()
{
    if(set_target_ == 0)
    {
       return;
    }
    int z = current_step_;
    Dir step = VDirToDir(movement_[z]);

   if(!TryMove(step))
   {
       this->Delete();
       return;
   }
   current_step_++;
   if(!(current_step_ < movement_.size()) && !reached_target )
   {
       current_step_ = 0;
       VDir temp = movement_[0];
       movement_.clear();
       reached_target = true;
       movement_.push_back(temp);
   }
   if(!(current_step_<movement_.size()) && reached_target )
   {
   current_step_ = 0;
   }   
}

bool Projectile::CheckPassable()
{
    id_ptr_on<Projectile> p = this->GetId();
    PassableLevel loc = GetPassable(dMove);
    if (loc != Passable::FULL)
    {
        SetPassable(dMove, Passable::FULL);
    }
    if (!CanPass(owner->GetPassable(dMove), passable_level))
    {
        owner->AttackByP(p);
        if (loc != Passable::FULL)
        {
            SetPassable(dMove, loc);
        }
        return false;
    }
    if (loc != Passable::FULL)
    {
        SetPassable(dMove, loc);
    }

    auto neighbour = owner->GetNeighbour(dMove);
    if (   !CanPass(neighbour->GetPassable(D_ALL), passable_level)
        || !CanPass(neighbour->GetPassable(helpers::revert_dir(dMove)), passable_level))
    {
        neighbour->AttackByP(p);
        return false;
    }
    
    return true;
}
void Projectile::Bump(id_ptr_on<IMovable> item)
{
    id_ptr_on<Projectile> p = this->GetId();
    item->AttackByP(p);
    this->Delete();
    
//TODO attackby()(things in other class related to this)
//TODO maybe it can apply froce depending on bullet speed but that should be considered        ApplyForce(DirToVDir[m->dMove]);
}
void Projectile::MakeMovementPattern(VDir target)
{
//TODO numbers to VDir!!!
//TODO maybe an enum for directions would be better I might change this
    set_target_=1;
    int x = target.x;
    int y = target.y;
    if(std::abs(x)>std::abs(y))
    {
        int x_moves = std::abs(x)-std::abs(y);
	int diagonal_moves = std::abs(y);
	if(x>=0 && y>=0)
        {
	    //east
            for(int e;x_moves>e;e++)
            {
            movement_.push_back(VD_RIGHT);
            }
            //northeast
            for(int i;diagonal_moves>i;i++)
            {
            movement_.push_back(VD_UPRight);
            }
            return;	
        }
	else if(x>0 && y<0)
        {
	    //east
            for(int e;x_moves>e;e++)
            {
            movement_.push_back(VD_RIGHT);
            }
            //southeast
            for(int i;diagonal_moves>i;i++)
            {
            movement_.push_back(VD_DOWNRight);
            }
            return;	
        }
	else if(x<0 && y>0)
        {
	    //west
            for(int e;x_moves>e;e++)
            {
            movement_.push_back(VD_LEFT);
            }
            //northwest
            for(int i;diagonal_moves>i;i++)
            {
            movement_.push_back(VD_UPLeft);
            }
            return;	
        }
	else if(x<=0 && y<=0)
        {
	    //west
            for(int e;x_moves>e;e++)
            {
            movement_.push_back(VD_LEFT);
            }
            //southwest
            for(int i;diagonal_moves>i;i++)
            {
            movement_.push_back(VD_DOWNLeft);
            }	
            return;
        }
	
	
    }
    else if(std::abs(x)<std::abs(y))
    {
        int y_moves = std::abs(y)-std::abs(x);
	int diagonal_moves = std::abs(x);
        if(x>=0 && y>=0)
        {
	    //north
            for(int e;y_moves>e;e++)
            {
            movement_.push_back(VD_UP);
            }
            //north-east
            for(int i;diagonal_moves>i;i++)
            {
            movement_.push_back(VD_UPRight);
            }
            return;	
        }
	else if(x>0 && y<0)
        {
	    //south
            for(int e;y_moves>e;e++)
            {
            movement_.push_back(VD_DOWN);
            }
            //southeast
            for(int i;diagonal_moves>i;i++)
            {
            movement_.push_back(VD_DOWNRight);
            }
            return;	
        }
	else if(x<0 && y>0)
        {
	    //north
            for(int e;y_moves>e;e++)
            {
            movement_.push_back(VD_UP);
            }
            //northwest
            for(int i;diagonal_moves>i;i++)
            {
            movement_.push_back(VD_UPLeft);
            }
            return;	
        }
	else if(x<=0 && y<=0)
        {
	    //south
            for(int e;y_moves>e;e++)
            {
            movement_.push_back(VD_DOWN);
            }
            //southwest
            for(int i;diagonal_moves>i;i++)
            {
            movement_.push_back(VD_DOWNLeft);
            }
            return;	
        }
    }
    else if(std::abs(x)==std::abs(y))
    {
	if(x>=0 && y>=0)
        {
            //northeast
            movement_.push_back(VD_UPRight);	
            return;
        }
	else if(x>0 && y<0)
        {
            //southeast
            movement_.push_back(VD_DOWNRight);
            return;
        }
	else if(x<0 && y>0)
        {
            //northwest
            movement_.push_back(VD_UPLeft);
            return;
        }
	else if(x<=0 && y<=0)
        {
            //southwest
            movement_.push_back(VD_DOWNLeft);
            return;
        }
	
    }
    else
    {
    this->Delete();
//never should go here but if it does we should delete that projectile
    }
}
void Projectile::AfterWorldCreation()
{
    IMainObject::AfterWorldCreation();
    SetFreq(1);
}


