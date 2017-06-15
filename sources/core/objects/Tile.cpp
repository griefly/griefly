#include <cassert>

#include "../Helpers.h"
#include "../Map.h"
#include "../Game.h"
#include "MaterialObject.h"
#include "Tile.h"
#include "movable/Movable.h"
#include "../atmos/AtmosGrid.h"

using namespace kv;

CubeTile::CubeTile()
    : position_(-1, -1, -1),
      sum_passable_all_(passable::FULL),
      sum_passable_up_(passable::FULL),
      sum_passable_down_(passable::FULL),
      sum_passable_left_(passable::FULL),
      sum_passable_right_(passable::FULL)
{
    // Nothing
}

bool CubeTile::CanTouch(IdPtr<MapObject> item) const
{
    if (!item.IsValid())
    {
        return false;
    }

    if (!item->GetOwner())
    {
        return false;
    }

    IdPtr<CubeTile> cube_tile = item->GetOwner();
    if (!cube_tile.IsValid())
    {
        return false;
    }

    const Position cube_tile_position = item->GetPosition();
    const int cube_tile_posx = cube_tile_position.x;
    const int cube_tile_posy = cube_tile_position.y;

    const int posx = position_.x;
    const int posy = position_.y;

    if (qAbs(posx - cube_tile_posx) > 1)
    {
        return false;
    }
    if (qAbs(posy - cube_tile_posy) > 1)
    {
        return false;
    }

    if (   (posx == cube_tile_posx)
        && (posy == cube_tile_posy))
    {
        return true;
    }

    if (posx == cube_tile_posx)
    {
        if (posy > cube_tile_posy)
        {
            return CanTouch(item, Dir::UP);
        }
        else
        {
            return CanTouch(item, Dir::DOWN);
        }
    }
    if (posy == cube_tile_posy)
    {
        if (posx > cube_tile_posx)
        {
            return CanTouch(item, Dir::LEFT);
        }
        else
        {
            return CanTouch(item, Dir::RIGHT);
        }
    }

    // Up Left
    if (   (posy > cube_tile_posy)
        && (posx > cube_tile_posx))
    {
        return    CanTouch(item, Dir::LEFT, Dir::UP)
               || CanTouch(item, Dir::UP, Dir::LEFT);
    }
    // Down Right
    if (   (posy < cube_tile_posy)
        && (posx < cube_tile_posx))
    {
        return    CanTouch(item, Dir::RIGHT, Dir::DOWN)
               || CanTouch(item, Dir::DOWN, Dir::RIGHT);
    }

    // Up Right
    if (   (posy > cube_tile_posy)
        && (posx < cube_tile_posx))
    {
        return    CanTouch(item, Dir::RIGHT, Dir::UP)
               || CanTouch(item, Dir::UP, Dir::RIGHT);
    }

    // Down Left
    if (   (posy < cube_tile_posy)
        && (posx > cube_tile_posx))
    {
        return    CanTouch(item, Dir::LEFT, Dir::DOWN)
               || CanTouch(item, Dir::DOWN, Dir::LEFT);
    }

    // It should not be reached
    return false;
}


bool CubeTile::CanTouch(IdPtr<MapObject> item, Dir dir) const
{
    if (!CanPass(GetPassable(dir), passable::BIG_ITEM))
    {
        return false;
    }

    // TODO: implementation is not perfect, but fine for a while
    if (!CanPass(item->GetPassable(RevertDir(dir)), passable::BIG_ITEM))
    {
        return true;
    }
    if (CanPass(item->GetOwner()->GetPassable(RevertDir(dir)), passable::BIG_ITEM))
    {
        return true;
    }
    return false;
}

bool CubeTile::CanTouch(IdPtr<MapObject> item, Dir first_dir, Dir second_dir) const
{
    if (!CanPass(GetPassable(first_dir), passable::BIG_ITEM))
    {
        return false;
    }

    auto tile = GetNeighbour(first_dir);

    if (   !CanPass(tile->GetPassable(RevertDir(first_dir)), passable::BIG_ITEM)
        || !CanPass(tile->GetPassable(RevertDir(Dir::ALL)), passable::BIG_ITEM))
    {
        return false;
    }

    if (!CanPass(tile->GetPassable(second_dir), passable::BIG_ITEM))
    {
        return false;
    }

    if (!CanPass(item->GetPassable(RevertDir(second_dir)), passable::BIG_ITEM))
    {
        return true;
    }

    if (CanPass(item->GetOwner()->GetPassable(RevertDir(second_dir)), passable::BIG_ITEM))
    {
        return true;
    }

    return false;
}

void CubeTile::MoveToDir(Dir dir, Position* position) const
{
    position->x += DirToVDir(dir).x;
    if (position->x >= GetGame().GetMap().GetWidth() ||
        position->x <= -1)
    {
        position->x -= DirToVDir(dir).x;
    }
    position->y += DirToVDir(dir).y;
    if (position->y >= GetGame().GetMap().GetHeight() ||
        position->y <= -1)
    {
        position->y -= DirToVDir(dir).y;
    }
    position->z += DirToVDir(dir).z;
    if (position->z >= GetGame().GetMap().GetDepth() ||
        position->z <= -1)
    {
        position->z -= DirToVDir(dir).z;
    }
}

bool CubeTile::Contains(IdPtr<MapObject> item) const
{
    for (auto it = inside_list_.begin(); it != inside_list_.end(); ++it)
    {
        if (it->Id() == item.Id())
        {
            return true;
        }
    }
    return false;
}

void CubeTile::Bump(IdPtr<Movable> item)
{
    if (GetTurf())
    {
        GetTurf()->Bump(item);
    }

    if (item->GetOwner().Id() == GetId())
    {
        for (auto it = inside_list_.begin(); it != inside_list_.end(); ++it)
            if (!CanPass((*it)->GetPassable(item->GetDir()), item->passable_level))
            {
                (*it)->Bump(item);
                return;
            }
        return;
    }

    for (auto it = inside_list_.begin(); it != inside_list_.end(); ++it)
        if (!CanPass((*it)->GetPassable(RevertDir(item->GetDir())), item->passable_level))
        {
            (*it)->Bump(item);
            return;
        }
    for (auto it = inside_list_.begin(); it != inside_list_.end(); ++it)
        if (!CanPass((*it)->GetPassable(Dir::ALL), item->passable_level))
        {
            (*it)->Bump(item);
            return;
        }
}

void CubeTile::BumpByGas(Dir dir, bool inside)
{
    if (GetTurf())
        GetTurf()->BumpByGas(dir);

    if (inside)
    {
        for (auto it = inside_list_.begin(); it != inside_list_.end(); ++it)
            if (!CanPass((*it)->GetPassable(dir), passable::AIR))
            {
                (*it)->BumpByGas(dir);
                return;
            }
        return;
    }

    for (auto it = inside_list_.begin(); it != inside_list_.end(); ++it)
        if (!CanPass((*it)->GetPassable(RevertDir(dir)), passable::AIR))
        {
            (*it)->BumpByGas(dir);
            return;
        }
    for (auto it = inside_list_.begin(); it != inside_list_.end(); ++it)
        if (!CanPass((*it)->GetPassable(Dir::ALL), passable::AIR))
        {
            (*it)->BumpByGas(dir);
            return;
        }
}

bool CubeTile::AddObject(IdPtr<MapObject> item_raw)
{
    IdPtr<MaterialObject> item = item_raw;
    if (!item.IsValid())
    {
        return false;
    }

    inside_list_.push_back(item);
    item->SetOwner(GetId());

    sum_passable_all_ = std::min(sum_passable_all_, item->GetPassable(Dir::ALL));
    sum_passable_up_ = std::min(sum_passable_up_, item->GetPassable(Dir::UP));
    sum_passable_down_ = std::min(sum_passable_down_, item->GetPassable(Dir::DOWN));
    sum_passable_left_ = std::min(sum_passable_left_, item->GetPassable(Dir::LEFT));
    sum_passable_right_ = std::min(sum_passable_right_, item->GetPassable(Dir::RIGHT));

    UpdateAtmosPassable();
    return true;
}
bool CubeTile::RemoveObject(IdPtr<MapObject> item_raw)
{
    IdPtr<MaterialObject> item = item_raw;
    if (!item.IsValid())
    {
        return false;
    }

    if (item == GetTurf())
    {
        SetTurf(0);
        return true;
    }

    auto itr = inside_list_.begin();
    while(itr != inside_list_.end())
    {
        if (itr->Id() == item->GetId())
        {
            inside_list_.erase(itr);
            UpdatePassable();
            return true;
        }
        ++itr;
    }
    return false;
}



IdPtr<MapObject> CubeTile::GetNeighbour(Dir direct) const
{
    return GetNeighbourImpl(direct);
}

IdPtr<CubeTile> CubeTile::GetNeighbourImpl(Dir direct) const
{
    Position new_position = GetPosition();
    MoveToDir(direct, &new_position);
    return GetGame().GetMap().At(new_position.x, new_position.y, new_position.z);
}

PassableLevel CubeTile::GetPassable(Dir direct) const
{
    switch (direct)
    {
    case Dir::UP:
        return sum_passable_up_;
    case Dir::DOWN:
        return sum_passable_down_;
    case Dir::LEFT:
        return sum_passable_left_;
    case Dir::RIGHT:
        return sum_passable_right_;
    case Dir::ALL:
        return sum_passable_all_;
    }
    return passable::FULL;
}

void CubeTile::UpdatePassable()
{
    sum_passable_all_ = passable::FULL;
    sum_passable_up_ = passable::FULL;
    sum_passable_down_ = passable::FULL;
    sum_passable_left_ = passable::FULL;
    sum_passable_right_ = passable::FULL;

    if (turf_.IsValid())
    {
        sum_passable_all_ = std::min(sum_passable_all_, turf_->GetPassable(Dir::ALL));
        sum_passable_up_ = std::min(sum_passable_up_, turf_->GetPassable(Dir::UP));
        sum_passable_down_ = std::min(sum_passable_down_, turf_->GetPassable(Dir::DOWN));
        sum_passable_left_ = std::min(sum_passable_left_, turf_->GetPassable(Dir::LEFT));
        sum_passable_right_ = std::min(sum_passable_right_, turf_->GetPassable(Dir::RIGHT));
    }
    for (auto it = inside_list_.begin(); it != inside_list_.end(); ++it)
    {
        sum_passable_all_ = std::min(sum_passable_all_, (*it)->GetPassable(Dir::ALL));
        sum_passable_up_ = std::min(sum_passable_up_, (*it)->GetPassable(Dir::UP));
        sum_passable_down_ = std::min(sum_passable_down_, (*it)->GetPassable(Dir::DOWN));
        sum_passable_left_ = std::min(sum_passable_left_, (*it)->GetPassable(Dir::LEFT));
        sum_passable_right_ = std::min(sum_passable_right_, (*it)->GetPassable(Dir::RIGHT));
    }

    UpdateAtmosPassable();
}

bool CubeTile::IsTransparent() const
{
    if (turf_.IsValid() && !turf_->IsTransparent())
    {
        return false;
    }
    for (auto it = inside_list_.begin(); it != inside_list_.end(); ++it)
    {
        if (!(*it)->IsTransparent())
        {
            return false;
        }
    }
    return true;
}

quint32 CubeTile::GetItemImpl(unsigned int hash)
{
    for (auto it = inside_list_.rbegin(); it != inside_list_.rend(); ++it)
    {
        if (FastIsType(hash, (*it)->GetTypeIndex()))
        {
            return it->Id();
        }
    }
    return 0;
}

void CubeTile::ForEach(std::function<void(IdPtr<MapObject>)> callback)
{
    InsideType copy_vector = inside_list_;

    // TODO: possible bug when callback invalidate some of vector object
    // ForEach callback may expect that all objects will be valid
    for (auto it = copy_vector.begin(); it != copy_vector.end(); ++it)
    {
        callback(*it);
    }
}

void CubeTile::UpdateAtmosPassable()
{
    const Dir dirs[5]
        = { Dir::ALL,
            Dir::UP,
            Dir::DOWN,
            Dir::LEFT,
            Dir::RIGHT };
    const AtmosInterface::Flags bit_dirs[5]
        = { atmos::CENTER_BLOCK,
            atmos::UP_BLOCK,
            atmos::DOWN_BLOCK,
            atmos::LEFT_BLOCK,
            atmos::RIGHT_BLOCK };

    AtmosInterface::Flags flags = atmos::CLEAR;
    for (int i = 0; i < 5; ++i)
    {
        if (!CanPass(GetPassable(dirs[i]), passable::AIR))
        {
            flags |= bit_dirs[i];
        }
    }

    if (turf_ && turf_->GetAtmosState() == atmos::SPACE)
    {
        flags |= atmos::SPACE_TILE;
    }
    if (inside_list_.empty())
    {
        flags |= atmos::NO_OBJECTS;
    }

    GetGame().GetAtmosphere().SetFlags(position_.x, position_.y, position_.z, flags);
}
