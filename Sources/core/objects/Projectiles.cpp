#include "Projectiles.h"

#include "../Helpers.h"
#include "Mob.h"

#include "../Game.h"

#include <assert.h>

Projectile::Projectile(size_t id) : IOnMapObject(id)
{
    lastMove = 0;
    SetPassable(D_ALL, Passable::AIR);
    tickSpeed = 0.25;
    pixSpeed = 1;
    dMove = D_DOWN;
    anchored = false;
    damage = 0;

    v_level = 5;
    SetSprite("icons/projectiles.dmi");
}

bool Projectile::TryMove(Dir direct)
{
    if (!CheckMoveTime())
    {
        return false;
    }
    if (anchored)
    {
        return false;
    }
    if (!Rotate(direct))
    {
        return false;
    }
    if (!CheckPassable())
    {
        return false;
    }
    return MainMove();
}

void Projectile::ProcessMovement()
{
    int z = current_step_;
    Dir step = VDirToDir(movement_[z]);

    TryMove(step);
    current_step_++;
    if(!(current_step_<movement_.size()) && !reached_target )
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

bool Projectile::CheckMoveTime()
{
    if ((static_cast<int>(MAIN_TICK) - lastMove) < tickSpeed)
    {
        return false;
    }
    return true;
}

bool Projectile::CheckPassable()
{
    PassableLevel loc = GetPassable(dMove);
    if (loc != Passable::FULL)
    {
        SetPassable(dMove, Passable::FULL);
    }
    if (!CanPass(owner->GetPassable(dMove), passable_level))
    {
        owner->Bump(GetId());
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
        neighbour->Bump(GetId());
        return false;
    }
    
    return true;
}

bool Projectile::Rotate(Dir dir)
{
    dMove = dir;
    return true;
}

bool Projectile::MainMove()
{
    auto new_owner = owner->GetNeighbour(dMove);
    if (new_owner == owner)
    {
        return false;
    }

    owner->RemoveItem(GetId());
    new_owner->AddItem(GetId());

    lastMove = static_cast<int>(MAIN_TICK);
    return true;
}


void Projectile::Bump(id_ptr_on<IMovable> item)
{
    item->AttackBy(this->GetOwner());
    Game* a  = static_cast<Game*>(GetGame().GetTrueGame());
    auto it = a->GetProjectileProcess().find(this->GetOwner().ret_id());
    a->GetProjectileProcess().erase(it);
    this->Delete();
    delete a;
//TODO attackby()(things in other class related to this)
//TODO maybe it can apply froce depending on bullet speed but that should be considered        ApplyForce(DirToVDir[m->dMove]);
}
void Projectile::Bump(id_ptr_on<Projectile> item)
{
    Game* a  = static_cast<Game*>(GetGame().GetTrueGame());
    auto it = a->GetProjectileProcess().find(item.ret_id());
    a->GetProjectileProcess().erase(it);
    auto it2 = a->GetProjectileProcess().find(this->GetOwner().ret_id());
    a->GetProjectileProcess().erase(it2);
    item->Delete();
    this->Delete();
    delete a;
    
//TODO attackby()(things in other class related to this)
//TODO maybe it can apply froce depending on bullet speed but that should be considered        ApplyForce(DirToVDir[m->dMove]);
}
void Projectile::Represent()
{
    Representation::Entity ent;
    ent.id = GetId();
    ent.pos_x = GetX();
    ent.pos_y = GetY();
    ent.vlevel = v_level;
    ent.view = *GetView();
    ent.dir = GetDir();
    GetRepresentation().AddToNewFrame(ent);
}
void Projectile::MakeMovementPattern(sf::Vector2i target)
{
//TODO numbers to VDir!!!
//TODO maybe an enum for directions would be better I might change this
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
        }
    }
    else if(std::abs(x)==std::abs(y))
    {
	if(x>=0 && y>=0)
        {
            //northeast
            movement_.push_back(VD_UPRight);	
        }
	else if(x>0 && y<0)
        {
            //southeast
            movement_.push_back(VD_DOWNRight);
        }
	else if(x<0 && y>0)
        {
            //northwest
            movement_.push_back(VD_UPLeft);
        }
	else if(x<0 && y<0)
        {
            //southwest
            movement_.push_back(VD_DOWNLeft);
        }
	
    }
    else
    {
    this->Delete();
    Game* a  = static_cast<Game*>(GetGame().GetTrueGame());
    auto it = a->GetProjectileProcess().find(this->GetOwner().ret_id());
    a->GetProjectileProcess().erase(it);
    delete a;
//never should go here but if it does we should delete that projectile
    }
}

