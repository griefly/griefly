#include "Projectiles.h"
#include "core/Helpers.h"

using namespace kv;

FastSerializer& operator<<(FastSerializer& file, const std::vector<Dir>& s)
{
    file << static_cast<quint32>(s.size());
    for (auto it : s)
    {
        file << it;
    }
    return file;
}

FastDeserializer& operator>>(FastDeserializer& file, std::vector<Dir>& s)
{
    quint32 size;
    file >> size;
    s.reserve(size);

    Dir dir;
    for (quint32 i = 0; i < size; ++i)
    {
        file >> dir;
        s.push_back(dir);
    }
    return file;
}

unsigned int hash(const std::vector<Dir>& content)
{
    unsigned int retval = 0;
    int i = 1;
    for (auto it = content.begin(); it != content.end(); ++it, ++i)
    {
        retval += hash(*it) * i;
    }
    return retval;
}


Projectile::Projectile()
{
    SetPassable(Dir::ALL, passable::FULL);
    damage_ = 0;
    burn_damage_ = 0;
    SetSprite("icons/projectiles.dmi");
    v_level = 6;
    current_step_ = 0;
    tick_speed_ = 1;
    passable_level = passable::SMALL_ITEM;
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
        human->Bump(GetId());
        Delete();
        return true;
    }
    return false;
}

bool Projectile::ProcessMovement()
{
    if (movement_.size() == 0)
    {
        KvAbort("Trajectory for the projectile is empty, critical error!");
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
            if (facing == Dir::LEFT || facing == Dir::RIGHT)
            {
                if (x > 0)
                {
                    MakeMovementLoops(abs_y, abs_x, Dir::RIGHT, Dir::UP);
                }
                else
                {
                    MakeMovementLoops(abs_y, abs_x, Dir::LEFT, Dir::UP);
                }
            }
            else
            {
                if (x > 0)
                {
                    MakeMovementLoops(abs_y, abs_x, Dir::UP, Dir::RIGHT);
                }
                else
                {
                    MakeMovementLoops(abs_y, abs_x, Dir::UP, Dir::LEFT);
                }
            }
        }
        else
        {
            if (facing == Dir::LEFT || facing == Dir::RIGHT)
            {
                if (x > 0)
                {
                    MakeMovementLoops(abs_y, abs_x, Dir::RIGHT, Dir::DOWN);
                }
                else
                {
                    MakeMovementLoops(abs_y, abs_x, Dir::LEFT, Dir::DOWN);
                }
            }
            else
            {
                if (x > 0)
                {
                    MakeMovementLoops(abs_y, abs_x, Dir::DOWN, Dir::RIGHT);
                }
                else
                {
                    MakeMovementLoops(abs_y, abs_x, Dir::DOWN, Dir::LEFT);
                }
            }
        }
    }
    else if (x >= 0 && y >= 0)
    {
       if (y > x)
       {
            MakeMovementLoops(abs_y, abs_x, Dir::DOWN, Dir::RIGHT);
       }
       else if (x > y)
       {
            MakeMovementLoops(abs_x, abs_y, Dir::RIGHT, Dir::DOWN);
       }
    }
    else if(x <= 0 && y <= 0)
    {   
       if (abs_y > abs_x  )
       {
           MakeMovementLoops(abs_y, abs_x, Dir::UP, Dir::LEFT);
       }
       else if (abs_x > abs_y)
       {
           MakeMovementLoops(abs_x, abs_y, Dir::LEFT, Dir::UP);
       }
    }
    else if (x < 0 && y > 0)
    {
        if (y > abs_x)
        {
             MakeMovementLoops(abs_y, abs_x, Dir::DOWN, Dir::LEFT);
        }
        else if (abs_x > abs_y)
        {
            MakeMovementLoops(abs_x, abs_y, Dir::LEFT, Dir::DOWN);
        }
    }
    else if (x > 0 && y < 0)
    {
        if (abs_y > x)
        {
            MakeMovementLoops(abs_y, abs_x, Dir::UP, Dir::RIGHT);
        }
        else if (x > abs_y )
        {
            MakeMovementLoops(abs_x, abs_y, Dir::RIGHT, Dir::UP);
        }
        
    }
    else
    {
        KvAbort("Critical error, MakeMovementPattern has reached unreachable place");
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
int Projectile::GetDamage()
{
    return damage_;
}
int Projectile::GetBurnDamage()
{
    return burn_damage_;
}


