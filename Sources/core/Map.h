#pragma once

#include <stddef.h>

#include <list>
#include <vector>

#include "Constheader.h"
#include "objects/Tile.h"
#include "Atmos.h"

#include "Interfaces.h"

class MapMaster;
class LOSfinder
{
public:
    LOSfinder(IMapMaster* map);
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

    IMapMaster* map_;
};

class SyncRandom;
class MapMaster : public IMapMaster
{
public:
    MapMaster(SyncRandom* sync_random);
    ~MapMaster();

    typedef id_ptr_on<CubeTile> SqType;

    virtual IAtmosphere& GetAtmosphere() override;

    virtual std::vector<std::vector<std::vector<SqType>>>& GetSquares() override;
    virtual const std::vector<std::vector<std::vector<SqType>>>& GetSquares() const override;

    virtual int GetWidth() const override;
    virtual int GetHeight() const override;
    virtual int GetDepth() const override;

    virtual std::list<point>* GetVisiblePoints() override;

    virtual void ResizeMap(int x, int y, int z) override;
    virtual void FillAtmosphere() override;

    virtual void Represent() override;

    virtual bool IsTileVisible(size_t tile_id) override;
    virtual bool IsTransparent(int posx, int posy, int posz = 0) override;

    virtual void CalculateVisisble(std::list<point>* retval, int posx, int posy, int posz = 0) override;

    virtual bool CheckBorders(const int* x, const int* y, const int* z) const override;
private:
    LOSfinder losf_;
    IAtmosphere* atmosphere_;
    std::vector<std::vector<std::vector<SqType>>> squares_;
    std::list<point>* visible_points_;
};
