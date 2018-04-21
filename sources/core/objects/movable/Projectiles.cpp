#include "Projectiles.h"

using namespace kv;

Projectile::Projectile()
{
    SetPassable(Dir::ALL, passable::FULL);
    damage_ = 0;
    burn_damage_ = 0;
    SetSprite("icons/projectiles.dmi");
    SetVisibleLevel(visible_level::ITEM + 1);
    current_step_ = 0;

    SetTickSpeed(1);
    SetPassableLevel(passable::SMALL_ITEM);
    harmless_ = true;
}

void Projectile::Process()
{
    Dir old_dir = GetDir();
    if (!ProcessMovement())
    {
        return;
    }
    harmless_ = false;
    if (GetDir() != old_dir)
    {
        ProcessMovement();
    }
}

bool Projectile::CheckPassable()
{   
    if (!harmless_)
    {
        if (CheckHumanTile())
        {
            return false;
        }
    }
    if (Movable::CheckPassable())
    {
        return true;
    }
    Delete();
    return false;
}

bool Projectile::CheckHumanTile()
{
    auto tile = GetOwner();
    if (IdPtr<Human> human = tile->GetItem<Human>())
    {
        human->Bump(DirToVDir(GetDir()), GetId());
        Delete();
        return true;
    }
    return false;
}

bool Projectile::ProcessMovement()
{
    if (movement_.size() == 0)
    {
        kv::Abort("Trajectory for the projectile is empty, critical error!");
    }
    Dir step = movement_[current_step_];
    Rotate(step);
    if(!CheckPassable())
    {
        return false;
    }
    current_step_ = (current_step_ + 1) % movement_.size();
    MainMove();
    return true;
}

void Projectile::MakeMovementPattern(Vector target, Dir facing)
{
    int x = target.x;
    int y = target.y;
    int abs_x = std::abs(x);
    int abs_y = std::abs(y);
    if (abs_x == abs_y)
    {
        if (y < 0)
        {
            if (facing == Dir::WEST || facing == Dir::EAST)
            {
                if (x > 0)
                {
                    MakeMovementLoops(abs_y, abs_x, Dir::EAST, Dir::NORTH);
                }
                else
                {
                    MakeMovementLoops(abs_y, abs_x, Dir::WEST, Dir::NORTH);
                }
            }
            else
            {
                if (x > 0)
                {
                    MakeMovementLoops(abs_y, abs_x, Dir::NORTH, Dir::EAST);
                }
                else
                {
                    MakeMovementLoops(abs_y, abs_x, Dir::NORTH, Dir::WEST);
                }
            }
        }
        else
        {
            if (facing == Dir::WEST || facing == Dir::EAST)
            {
                if (x > 0)
                {
                    MakeMovementLoops(abs_y, abs_x, Dir::EAST, Dir::SOUTH);
                }
                else
                {
                    MakeMovementLoops(abs_y, abs_x, Dir::WEST, Dir::SOUTH);
                }
            }
            else
            {
                if (x > 0)
                {
                    MakeMovementLoops(abs_y, abs_x, Dir::SOUTH, Dir::EAST);
                }
                else
                {
                    MakeMovementLoops(abs_y, abs_x, Dir::SOUTH, Dir::WEST);
                }
            }
        }
    }
    else if (x >= 0 && y >= 0)
    {
       if (y > x)
       {
            MakeMovementLoops(abs_y, abs_x, Dir::SOUTH, Dir::EAST);
       }
       else if (x > y)
       {
            MakeMovementLoops(abs_x, abs_y, Dir::EAST, Dir::SOUTH);
       }
    }
    else if(x <= 0 && y <= 0)
    {   
       if (abs_y > abs_x  )
       {
           MakeMovementLoops(abs_y, abs_x, Dir::NORTH, Dir::WEST);
       }
       else if (abs_x > abs_y)
       {
           MakeMovementLoops(abs_x, abs_y, Dir::WEST, Dir::NORTH);
       }
    }
    else if (x < 0 && y > 0)
    {
        if (y > abs_x)
        {
             MakeMovementLoops(abs_y, abs_x, Dir::SOUTH, Dir::WEST);
        }
        else if (abs_x > abs_y)
        {
            MakeMovementLoops(abs_x, abs_y, Dir::WEST, Dir::SOUTH);
        }
    }
    else if (x > 0 && y < 0)
    {
        if (abs_y > x)
        {
            MakeMovementLoops(abs_y, abs_x, Dir::NORTH, Dir::EAST);
        }
        else if (x > abs_y )
        {
            MakeMovementLoops(abs_x, abs_y, Dir::EAST, Dir::NORTH);
        }
        
    }
    else
    {
        kv::Abort("Critical error, MakeMovementPattern has reached unreachable place");
    }
}
void Projectile::MakeMovementLoops(int d1_number, int d2_number, Dir d1, Dir d2)
{
    int shuffle = 1;
    int z = (2 * d2_number) - 1;
    for (int i = 0; i < d2_number + d1_number; i++)
    {
        if (shuffle == 1 || z < i)
        {
            movement_.push_back(d1);
            shuffle = 0;
        }
        else if (shuffle == 0)
        {
            movement_.push_back(d2);
            shuffle = 1;
        }
    }
}

void Projectile::AfterWorldCreation()
{
    Movable::AfterWorldCreation();
    SetFreq(1);
}
