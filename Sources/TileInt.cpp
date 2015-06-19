#include <assert.h>

#include "helpers.h"
#include "MapClass.h"
#include "OnMapInt.h"
#include "TileInt.h"
#include "IMovable.h"

CubeTile::CubeTile(size_t id) : IOnMapBase(id)
{
    turf_ = 0;

    posx_ = -1;
    posy_ = -1;
    posz_ = -1;

    sum_passable_all_ = -1;
    sum_passable_up_ = -1;
    sum_passable_down_ = -1;
    sum_passable_left_ = -1;
    sum_passable_right_ = -1;
}

bool CubeTile::CanTouch(id_ptr_on<IOnMapBase> item, int range) const
{
    if (!item.valid())
        return false;

    if (!item->GetOwner())
        return false;

    auto cube_tile = item->GetOwner();
    if (!cube_tile)
        return false;

    int x_begin = posx_ - range;
    if (x_begin < 0)
        x_begin = 0;
    int y_begin = posy_ - range;
    if (y_begin < 0)
        y_begin = 0;
    
    int x_end = posx_ + range;
    if (x_end >= GetMapMaster()->GetMapW())
        x_end = GetMapMaster()->GetMapW() - 1;
    int y_end = posy_ + range;
    if (y_end >= GetMapMaster()->GetMapH())
        y_end = GetMapMaster()->GetMapH() - 1;

    // TODO
    // check something like 
    //       xxx     o will be touchable (and its wrond)
    //       xox     maybe whatever?
    //       xxx  

    if (cube_tile->GetX() < x_begin)
        return false;
    if (cube_tile->GetX() > x_end)
        return false;
    if (cube_tile->GetY() < y_begin)
        return false;
    if (cube_tile->GetY() > y_end)
        return false;

    return true;
}

bool CubeTile::Contains(id_ptr_on<IOnMapBase> item) const
{
    for (auto it = inside_list_.begin(); it != inside_list_.end(); ++it)
        if (it->ret_id() == item.ret_id())
            return true;
    return false;
}

void CubeTile::Bump(id_ptr_on<IMovable> item)
{
    if (GetTurf())
        GetTurf()->Bump(item);

    if (item->GetOwner().ret_id() == GetId())
    {
        for (auto it = inside_list_.begin(); it != inside_list_.end(); ++it)
            if (!CanPass((*it)->GetPassable(item->dMove), item->passable_level))
            {
                (*it)->Bump(item);
                return;
            }
        return;
    }

    for (auto it = inside_list_.begin(); it != inside_list_.end(); ++it)
        if (!CanPass((*it)->GetPassable(helpers::revert_dir(item->dMove)), item->passable_level))
        {
            (*it)->Bump(item);
            return;
        }
    for (auto it = inside_list_.begin(); it != inside_list_.end(); ++it)
        if (!CanPass((*it)->GetPassable(D_ALL), item->passable_level))
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
            if (!CanPass((*it)->GetPassable(dir), Passable::AIR))
            {
                (*it)->BumpByGas(dir);
                return;
            }
        return;
    }

    for (auto it = inside_list_.begin(); it != inside_list_.end(); ++it)
        if (!CanPass((*it)->GetPassable(helpers::revert_dir(dir)), Passable::AIR))
        {
            (*it)->BumpByGas(dir);
            return;
        }
    for (auto it = inside_list_.begin(); it != inside_list_.end(); ++it)
        if (!CanPass((*it)->GetPassable(D_ALL), Passable::AIR))
        {
            (*it)->BumpByGas(dir);
            return;
        }
}

bool CubeTile::AddItem(id_ptr_on<IOnMapBase> item_raw)
{
    id_ptr_on<IOnMapObject> item = item_raw;
    if (!item.valid())
        return false;
    auto itr = inside_list_.begin();
    while(itr != inside_list_.end())
    {
        if (itr->ret_id() == item.ret_id())
        {
            SYSTEM_STREAM << "ALERT! " << item.ret_id() << " double added!\n";
            SDL_Delay(5000);
        }
        if (itr->ret_id() > item.ret_id())
            break;
        ++itr;
    }
    InsideType::iterator locit = itr;
    inside_list_.insert(locit, item);
    item->SetOwner(GetId());

    sum_passable_all_ = std::min(sum_passable_all_, item->GetPassable(D_ALL));
    sum_passable_up_ = std::min(sum_passable_up_, item->GetPassable(D_UP));
    sum_passable_down_ = std::min(sum_passable_down_, item->GetPassable(D_DOWN));
    sum_passable_left_ = std::min(sum_passable_left_, item->GetPassable(D_LEFT));
    sum_passable_right_ = std::min(sum_passable_right_, item->GetPassable(D_RIGHT));

    return true;
}
bool CubeTile::RemoveItem(id_ptr_on<IOnMapBase> item_raw)
{
    id_ptr_on<IOnMapObject> item = item_raw;
    if (!item.valid())
        return false;

    if (item == GetTurf())
    {
        SetTurf(0);
        return true;
    }

    auto itr = inside_list_.begin();
    while(itr != inside_list_.end())
    {
        if (itr->ret_id() == item->GetId())
        {
            inside_list_.erase(itr);
            UpdatePassable();
            return true;
        }
        ++itr;
    }
    return false;
}



id_ptr_on<IOnMapBase> CubeTile::GetNeighbour(Dir direct)
{
    return GetNeighbourImpl(direct);
}

id_ptr_on<CubeTile> CubeTile::GetNeighbourImpl(Dir direct)
{
    int new_x = posx_;
    int new_y = posy_;
    int new_z = posz_;
    helpers::move_to_dir(direct, &new_x, &new_y, &new_z);
    return GetMapMaster()->squares[new_x][new_y][new_z];
}

PassableLevel CubeTile::GetPassable(Dir direct) const
{
    if (sum_passable_all_ == -1)
    {
        UpdatePassable();
    }
    switch (direct)
    {
    case D_UP:    return sum_passable_up_;
    case D_DOWN:  return sum_passable_down_;
    case D_LEFT:  return sum_passable_left_;
    case D_RIGHT: return sum_passable_right_;
    case D_ALL:   return sum_passable_all_;
    }
    return Passable::FULL;
}

void CubeTile::UpdatePassable() const
{
    sum_passable_all_ = Passable::FULL;
    sum_passable_up_ = Passable::FULL;
    sum_passable_down_ = Passable::FULL;
    sum_passable_left_ = Passable::FULL;
    sum_passable_right_ = Passable::FULL;

    if (turf_.valid())
    {
        sum_passable_all_ = std::min(sum_passable_all_, turf_->GetPassable(D_ALL));
        sum_passable_up_ = std::min(sum_passable_up_, turf_->GetPassable(D_UP));
        sum_passable_down_ = std::min(sum_passable_down_, turf_->GetPassable(D_DOWN));
        sum_passable_left_ = std::min(sum_passable_left_, turf_->GetPassable(D_LEFT));
        sum_passable_right_ = std::min(sum_passable_right_, turf_->GetPassable(D_RIGHT));
    }
    for (auto it = inside_list_.begin(); it != inside_list_.end(); ++it)
    {
        sum_passable_all_ = std::min(sum_passable_all_, (*it)->GetPassable(D_ALL));
        sum_passable_up_ = std::min(sum_passable_up_, (*it)->GetPassable(D_UP));
        sum_passable_down_ = std::min(sum_passable_down_, (*it)->GetPassable(D_DOWN));
        sum_passable_left_ = std::min(sum_passable_left_, (*it)->GetPassable(D_LEFT));
        sum_passable_right_ = std::min(sum_passable_right_, (*it)->GetPassable(D_RIGHT));
    }
}

bool CubeTile::IsTransparent() const
{
    if (turf_.valid() && !turf_->IsTransparent())
        return false;
    for (auto it = inside_list_.begin(); it != inside_list_.end(); ++it)
        if (!(*it)->IsTransparent())
            return false;
    return true;
}

size_t CubeTile::GetItemImpl(unsigned int hash)
{
    for (auto it = inside_list_.rbegin(); it != inside_list_.rend(); ++it)
        if (FastIsType(hash, (*it)->RT_ITEM()))
            return it->ret_id();
    return 0;
}

void CubeTile::ForEach(std::function<void(id_ptr_on<IOnMapBase>)> callback)
{
    InsideType copy_vector = inside_list_;

    for (auto it = copy_vector.begin(); it != copy_vector.end(); ++it)
        callback(*it);
}

void CubeTile::LoadInMap()
{
    GetMapMaster()->squares[posx_][posy_][posz_] = GetId();
}
