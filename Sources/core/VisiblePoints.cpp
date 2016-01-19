#include "VisiblePoints.h"

#include "Game.h"
#include "objects/Tile.h"
#include "Map.h"

const std::list<point>* GetVisible()
{
    if (!IsGameValid())
    {
        return nullptr;
    }
    return GetGame().GetVisiblePoints();
}

bool IsTileVisible(size_t tile_id)
{
    auto l = GetVisible();
    if (!l)
    {
        return false;
    }
    if (!IsMapValid())
    {
        return false;
    }
    for (auto it = l->begin(); it != l->end(); ++it)
    {
        if (tile_id == GetMap().squares[it->posx][it->posy][it->posz].ret_id())
        {
            return true;
        }
    }
    return false;
}
