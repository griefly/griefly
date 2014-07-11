#include <assert.h>

#include "helpers.h"
#include "MapClass.h"
#include "OnMapInt.h"
#include "TileInt.h"
#include "IMovable.h"

CubeTile::CubeTile()
{
    turf_ = 0;

    posx_ = -1;
    posy_ = -1;
    posz_ = -1;
}

bool CubeTile::CanTouch(id_ptr_on<IOnMapBase> item, int range) const
{
    if (!item.valid())
        return false;
    int x_begin = posx_ - range;
    if (x_begin < 0)
        x_begin = 0;
    int y_begin = posy_ - range;
    if (y_begin)
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

    for (int i = x_begin; i <= x_end; ++i)
        for (int j = y_begin; j <= y_end; ++j)
        {
            auto tile = GetMapMaster()->squares[i][j][posz_];
            if (tile->Contains(item))
                return true;
            if (tile->GetTurf() == item)
                return true;
        }

    return false;
}

bool CubeTile::Contains(id_ptr_on<IOnMapBase> item) const
{
    for (auto it = inside_list_.begin(); it != inside_list_.end(); ++it)
        if (it->ret_id() == item.ret_id())
            return true;
    return false;
}

bool CubeTile::AddItem(id_ptr_on<IOnMapBase> item_raw)
{
    // TODO: IOnMapBase => IMovable
    id_ptr_on<IOnMapItem> item = item_raw;
    if (!item.valid())
        return false;
    auto itr = inside_list_.begin();
    while(itr != inside_list_.end())
    {
        if (itr->ret_id() == item.ret_id())
        {
            SYSTEM_STREAM << "ALERT! " << item.ret_id() << " double added!\n";
            SDL_Delay(5000);
            int* poc = 0;
            *poc = 1;
        }
        if (itr->ret_id() > item.ret_id())
            break;
        ++itr;
    }
    InsideType::iterator locit = itr;
    inside_list_.insert(locit, item);
    item->SetOwner(GetId());
    return true;
}
bool CubeTile::RemoveItem(id_ptr_on<IOnMapBase> item_raw)
{
    id_ptr_on<IOnMapItem> item = item_raw;
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
            return true;
        }
        ++itr;
    }
    return false;
}



id_ptr_on<IOnMapBase> CubeTile::GetNeighbour(Dir direct)
{
    int new_x = posx_;
    int new_y = posy_;
    int new_z = posz_;
    helpers::move_to_dir(direct, &new_x, &new_y, &new_z);
    return GetMapMaster()->squares[new_x][new_y][new_z];
}

bool CubeTile::IsPassable() const
{
    if (turf_.valid() && !turf_->IsPassable())
        return false;
    for (auto it = inside_list_.begin(); it != inside_list_.end(); ++it)
        if (!(*it)->IsPassable())
            return false;
    return true;
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
    for (auto it = inside_list_.begin(); it != inside_list_.end(); ++it)
        callback(*it);
}

void CubeTile::LoadInMap()
{
    GetMapMaster()->squares[posx_][posy_][posz_] = GetId();
}
