#include "Projectiles.h"
#include "../Helpers.h"


Projectile::Projectile(size_t id) : IMovable(id)
{
    SetPassable(D_ALL, Passable::AIR);
    tickSpeed = 0.25;
    damage = 0;
    SetSprite("icons/projectiles.dmi");
    v_level = 5;
    got_bored_ = 0;
    set_target_ = 0;
}
void Projectile::Process()
{
   if(set_target_ == 0)
   {
      return;
   }
   Dir step = CalculateTrajectoryMove();
   Rotate(step);
   if(!CheckPassable())
   {
       this->Delete();
       return;
   }
   MainMove(); 
}

bool Projectile::CheckPassable()
{
    id_ptr_on<Projectile> p = this->GetId();
    PassableLevel loc = GetPassable(GetDir());
    if (loc != Passable::FULL)
    {
        SetPassable(GetDir(), Passable::FULL);
    }
    if (!CanPass(owner->GetPassable(GetDir()), passable_level))
    {
        owner->Bump(p);
        if (loc != Passable::FULL)
        {
            SetPassable(GetDir(), loc);
        }
        return false;
    }
    if (loc != Passable::FULL)
    {
        SetPassable(GetDir(), loc);
    }

    auto neighbour = owner->GetNeighbour(GetDir());
    if (   !CanPass(neighbour->GetPassable(D_ALL), passable_level)
        || !CanPass(neighbour->GetPassable(helpers::revert_dir(GetDir())), passable_level))
    {
        neighbour->Bump(p);
        return false;
    }
    
    return true;
}
void Projectile::Bump(id_ptr_on<IMovable> item)
{
    id_ptr_on<Projectile> p = this->GetId();
    item->Bump(p);
    this->Delete();
   
//TODO maybe it can apply froce depending on bullet speed but that should be considered        ApplyForce(DirToVDir[m->dMove]);
}
void Projectile::MakeMovementPattern(VDir target,id_ptr_on<Human> someone)
{
    set_target_=1;
    target_ = target;
    int x = target.x;
    int y = target.y;
    int abs_x = std::abs(x);
    int abs_y = std::abs(y);
    shooter_ = someone;
    Dir facing = someone->GetDir();
    if(abs_x == abs_y)
    {
        if(y<0)
        {
            if(facing == 0|| facing == 1)
            {
                if(x>0)
                {
                    direction = D_RIGHT;
                    cases_ = 3;
                }
                else
                {
                    direction = D_LEFT;
                    cases_ = 6;
                }
            }
            else
            {
                if(x>0)
                {
                    direction = D_UP;
                    cases_ = 4;
                }
                else
                {
                    direction = D_UP;
                    cases_ = 5;
                }
            }
        }
        else
        {
        if(facing == 0|| facing == 1)
            {
                if(x>0)
                {
                    direction = D_RIGHT;
                    cases_ = 2;
                }
                else
                {
                    direction = D_LEFT;
                    cases_ = 7;
                }
            }
            else
            {
                if(x>0)
                {
                    direction = D_DOWN;
                    cases_ = 1;
                }
                else
                {
                    direction = D_DOWN;
                    cases_ = 8;
                }
            }
        }
    }
    else if(x >= 0 && y >= 0)
    {
        if(x != 0)
        {
            if((float)y / (float)x > 1 )
            {
                direction = D_DOWN;
                cases_ = 1;
            }
            else if((float)y / (float)x < 1)
            {
                direction = D_RIGHT;
                cases_ = 2; 
            }
       }
       else
       {
           direction = D_DOWN;
           cases_ = 1; 
       }
    }
    else if(x <= 0 && y <= 0)
    {   
        if(x != 0)
        {
             if((float)y / (float)x > 1 )
             {
                 direction = D_UP;
                 cases_ = 5;
             }
             else if((float)y / (float)x < 1)
             {
                 direction = D_LEFT;
                 cases_ = 6;
             }
        }
        else
        {
            direction = D_UP;
            cases_ = 5;
        }
    }
    else if(x < 0 && y > 0)
    {
        if((float)y / (float)x  < -1 )
        {
             direction = D_DOWN;
             cases_ = 8;
        }
        else if(y / x > -1)
        {
            direction = D_LEFT;
            cases_ = 7;
        }
    }
    else if(x > 0 && y < 0)
    {
        if((float)y / (float)x  < -1 )
        {
            direction = D_UP;
            cases_ = 4;
        }
        else if((float)y / (float)x  > -1 )
        {
            direction = D_RIGHT;
            cases_ = 3;
        }
        
    }
    else
    {
        this->Delete();
        return;
//never should go here but if it does we should delete that projectile
    }
    shooter_->Rotate(direction);
}
Dir Projectile::CalculateTrajectoryMove()
{
    if(reached_target)
    {
        return direction;
    }
    if(!reached_target  && target_.x == 0 && target_.y == 0)
    {
         reached_target = true;
         return direction;
    }
    switch(cases_)
    {
    case(1):
         if(got_bored_ == 1 || target_.x == 0)
         {
             target_.y--;
             got_bored_=2;
             return D_DOWN;
         }
         else if(got_bored_ == 2)
         {
             target_.x--; 
             got_bored_=1;
             return D_RIGHT;
         }
         else if(got_bored_ == 0)
         {
             target_.x--;
             got_bored_=1;
             return D_RIGHT;
         }
    case(2):
         if(got_bored_ == 1||target_.y == 0)
         {
             target_.x--;
             got_bored_=2;
             return D_RIGHT;
         }
         else if(got_bored_ == 2 )
         {
             target_.y--;
             got_bored_=1;
             return D_DOWN;
         }
         else if(got_bored_ == 0)
         {
             target_.y--;
             got_bored_=1;
             return D_DOWN;
         }
    case(3):
         if(got_bored_ == 1 || target_.y==0)
         {
             target_.x--;
             got_bored_=2;
             return D_RIGHT;
         }
         else if(got_bored_ == 2)
         {
             target_.y++;
             got_bored_=1;
             return D_UP;
         }
         else if(got_bored_ == 0)
         {
             target_.y++;
             got_bored_=1;
             return D_UP;
         }
    case(4):
         if(got_bored_ == 1 || target_.x==0)
         {
             target_.y++;
             got_bored_=2;
             return D_UP;
         }
         else if(got_bored_ == 2)
         {
             target_.x--;
             got_bored_=1;
             return D_RIGHT;
         }
         else if(got_bored_ == 0)
         {
             target_.x--;
             got_bored_=1;
             return D_RIGHT;
         }
    case(5):
         if(got_bored_ == 1 || target_.x ==0)
         {
             target_.y++;
             got_bored_=2;
             return D_UP;
         }
         else if(got_bored_ == 2)
         {
             target_.x++;
             got_bored_=1;
             return D_LEFT;
         }
         else if(got_bored_ == 0)
         {
             target_.x++;
             got_bored_=1;
             return D_LEFT;
         }
    case(6):
         if(got_bored_ == 1 || target_.y == 0)
         {
             target_.x++;
             got_bored_=2;
             return D_LEFT;
         }
         else if(got_bored_ == 2)
         {
             target_.y++;
             got_bored_=1;
             return D_UP;
         }
         else if(got_bored_ == 0)
         {
             target_.y++;
             got_bored_=1;
             return D_UP;
         }
    case(7):
         if(got_bored_ == 1 || target_.y==0)
         {
             target_.x++;
             got_bored_=2;
             return D_LEFT;
         }
         else if(got_bored_ == 2)
         {
             target_.y--;
             got_bored_=1;
             return D_DOWN;
         }
         else if(got_bored_ == 0)
         {
             target_.y--;
             got_bored_=1;
             return D_DOWN;
         }
    case(8):
         if(got_bored_ == 1|| target_.x==0)
         {
             target_.y--;
             got_bored_=2;
             return D_DOWN;
         }
         else if(got_bored_ == 2)
         {
             target_.x++;
             got_bored_=1;
             return D_LEFT;
         }
         else if(got_bored_ == 0)
         {
             target_.x++;
             got_bored_=1;
             return D_LEFT;
         }
    }    
}
void Projectile::AfterWorldCreation()
{
    IMovable::AfterWorldCreation();
    SetFreq(1);
}

