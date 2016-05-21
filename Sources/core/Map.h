#pragma once

#include <stddef.h>

#include <list>
#include <vector>

#include "Constheader.h"
#include "objects/Tile.h"
#include "Atmos.h"

class MapMaster;
class LOSfinder
{
public:
    LOSfinder(MapMaster* map);
    std::list<point>* CalculateVisisble(std::list<point>* retval, int posx, int posy, int posz = 0);
private:
    int pos2corner(int pos);
    int corner2pos(int corner);
    int sign(int value);
    bool check_corner(point p);
    point corner_point2point(point p);
    bool is_transparent(point p);
    bool bresen_x(point source, point target);
    bool bresen_y(point source, point target);
    bool ray_trace(point source, point target);
    void mark_tiles_of_corner_as_visible(
            std::list<point>* retlist,
            point at,
            point center,
            char visibility[]);
    void Clear();
    std::list<point> worklist;

    MapMaster* map_;
};

class SyncRandom;
class MapMaster
{
public:
    MapMaster(SyncRandom* random);
    ~MapMaster();

    typedef id_ptr_on<CubeTile> SqType;

    Atmosphere& GetAtmosphere()
    {
        return atmosphere_;
    }

    std::vector<std::vector<std::vector<SqType>>>& GetSquares()
    {
        return squares_;
    }

    const std::vector<std::vector<std::vector<SqType>>>& GetSquares() const
    {
        return squares_;
    }

    int GetWidth() const
    {
        return squares_.size();
    }
    int GetHeight() const
    {
        return squares_[0].size();
    }
    int GetDepth() const
    {
        return squares_[0][0].size();
    }

    void MoveToDir(Dir dir, int* x, int* y, int* z = nullptr) const
    {
        if (x)
        {
            *x += DirToVDir[dir].x;
            if (*x >= GetWidth() ||
                *x <= -1)
            {
                *x -= DirToVDir[dir].x;
            }
        }
        if (y)
        {
            *y += DirToVDir[dir].y;
            if (*y >= GetHeight() ||
                *y <= -1)
            {
                *y -= DirToVDir[dir].y;
            }
        }
        if (z)
        {
            *z += DirToVDir[dir].z;
            if (*z >= GetDepth() ||
                *z <= -1)
            {
                *z -= DirToVDir[dir].z;
            }
        }
    }

    bool CheckBorders(const int* x, const int* y, const int* z) const
    {
        if (x)
        {
            if (*x >= GetWidth())
            {
                return false;
            }
            if (*x < 0)
            {
                return false;
            }
        }
        if (y)
        {
            if (*y >= GetHeight())
            {
                return false;
            }
            if (*y < 0)
            {
                return false;
            }
        }
        if (z)
        {
            if (*z >= GetDepth())
            {
                return false;
            }
            if (*z < 0)
            {
                return false;
            }
        }
        return true;
    }

    void ResizeMap(int x, int y, int z);

    void Represent();

    bool IsTransparent(int posx, int posy, int posz = 0);

    void FillAtmosphere();

    PassableLevel GetPassable(int posx, int posy, int posz = 0, Dir direct = D_ALL);
    
    bool IsTileVisible(size_t tile_id);

    std::list<point>* GetVisiblePoints() { return visible_points_; }

    LOSfinder losf;
private:
    Atmosphere atmosphere_;
    std::vector<std::vector<std::vector<SqType>>> squares_;
    std::list<point>* visible_points_;
};
