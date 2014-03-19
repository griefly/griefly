#include <assert.h>

#include "helpers.h"
#include "MapClass.h"
#include "OnMapInt.h"
#include "TileInt.h"

CubeTile::CubeTile()
{
    posx_ = -1;
    posy_ = -1;
    posz_ = -1;
}
bool CubeTile::AddItem(id_ptr_on<IOnMapBase> item_raw)
{
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
            assert(false);
        }
        if (((*itr)->v_level == item->v_level && itr->ret_id() > item.ret_id())
           || (*itr)->v_level > item->v_level)
            break;
        ++itr;
    }
    InsideType::const_iterator locit = itr;
    inside_list_.insert(locit, item);
    item->SetOwner(id);
    return true;
}
bool CubeTile::RemoveItem(id_ptr_on<IOnMapBase> item_raw)
{
    id_ptr_on<IOnMapItem> item = item_raw;
    if (!item.valid())
        return false;
    auto itr = inside_list_.begin();
    while(itr != inside_list_.end())
    {
        if (itr->ret_id() == item->id)
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
    return IMainItem::map->squares[new_x][new_y][new_z];
}

bool CubeTile::IsPassable() const
{
    for (auto it = inside_list_.begin(); it != inside_list_.end(); ++it)
        if (!(*it)->IsPassable())
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
    map->squares[posx_][posy_][posz_] = id;
}