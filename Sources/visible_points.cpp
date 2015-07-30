#include "visible_points.h"

#include "Manager.h"
#include "TileInt.h"
#include "MapClass.h"

const std::list<point>* GetVisible()
{
    if (GetManager())
        return GetManager()->visiblePoint;
    return nullptr;
}

bool IsTileVisible(size_t tile_id)
{
    auto l = GetVisible();
    if (!l)
        return false;
    if (!GetMapMaster())
        return false;
    for (auto it = l->begin(); it != l->end(); ++it)
    {
        if (tile_id == GetMapMaster()->squares[it->posx][it->posy][it->posz].ret_id())
            return true;
    }
    return false;
}
