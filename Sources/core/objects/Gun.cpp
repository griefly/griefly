#include "Gun.h"
#include "Tile.h"

Gun::Gun(size_t id) : Item(id)
{
    SetSprite("icons/guns.dmi");
    ammunition_ = 1;
    max_ammunition_ = 1;
}

bool Gun::UseAmmo()
{
    if (ammunition_ <= 0)
    {
        ammunition_ = 0;
        return false;
    }
    --ammunition_;
    return true;
}

void Gun::ShootImpl(VDir target, const std::string& sound, const std::string& projectile_type, const std::string& casing_type)
{
    id_ptr_on<CubeTile> tile = GetOwner()->GetOwner();
    id_ptr_on<IMovable> shooter = GetOwner();
    if (tile.valid())
    {	
        if (UseAmmo())
        {
            int x = target.x;
            int y = target.y;
            Dir shooting_direction;
            Dir facing = shooter->GetDir();
            if(std::abs(y) != std::abs(x))
            {
                shooting_direction = VDirToDir(target);
            }
            else
            {
                if (y < 0)
                {
                    if (facing == D_LEFT || facing == D_RIGHT)
                    {
                        if (x > 0)
                        {
                            shooting_direction = D_RIGHT;
                        }
                        else
                        {
                            shooting_direction = D_LEFT;
                        }
                    }
                    else
                    {
                        shooting_direction = D_UP;
                    }
                }
                else
                {
                    if (facing == D_LEFT || facing == D_RIGHT)
                    {
                        if (x > 0)
                        {
                            shooting_direction = D_RIGHT;
                        }
                        else
                        {
                            shooting_direction = D_LEFT;
                        }
                    }
                    else
                    {
                        shooting_direction = D_DOWN;
                    }
                }
            }
            shooter->Rotate(shooting_direction);
            auto projectile = Create<Projectile>(projectile_type, tile->GetNeighbour(shooting_direction));
            projectile->MakeMovementPattern(target, facing);
            PlaySoundIfVisible(sound, tile.ret_id());
            if (!casing_type.empty())
            {
                Dir dir = GetRand() % 4;
                id_ptr_on<Item> casing = Create<Item>(casing_type, tile.ret_id());
                casing->Rotate(dir);
            } 	
        }
        else
        {
            PlaySoundIfVisible("empty.ogg", tile.ret_id()); 
            // the *click* text from ss13
        }
    }	
}

void Gun::Shoot(VDir target)
{
}

bool Gun::AddAmmo()
{
    if (ammunition_ < max_ammunition_)
    {
        ++ammunition_;
        return true;
    }
    return false;
}
void Gun::AttackBy(id_ptr_on<Item> item)
{
    if (id_ptr_on<AmmunitionBox> box = item)
    {
        if (box->CheckBullets())
        {
            if (AddAmmo())
            {
                box->RemoveBullet();
                return;
            }
        }
        // message which tells the player that the box is empty 
    }
}

bool Gun::Targetable(id_ptr_on<IOnMapBase> item)
{
    if (!item.valid())
    {
        return false;
    }

    if (!item->GetOwner())
    {
        return false;
    }

    id_ptr_on<CubeTile> cube_tile = item->GetOwner();
    if (!cube_tile.valid())
    {
        return false;
    }

    return true;
}

VDir Gun::TargetTileLoc(id_ptr_on<IOnMapBase> item) const
{
    id_ptr_on<CubeTile> cube_tile = item->GetOwner();
    VDir f;
    f.x = (cube_tile->GetX() - GetOwner()->GetX());
    f.y = (cube_tile->GetY() - GetOwner()->GetY());
    f.z = 0;
    return f;
}
