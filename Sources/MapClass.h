#pragma once

#include <list>
#include <vector>

#include "HelperStructers.h"
#include "constheader.h"
#include "TileInt.h"

class CPathFinder
{
public:
    std::list<Dir> calculatePath(int fromPosx, int fromPosy, int toPosx, int toPosy, int toPosz = 0);
private:
    std::list<square*> openList;
    std::vector<std::vector<square> > squares;
    void clearPathfinding();
    void addNear(int posx, int posy, int toPosx, int toPosy);
    int calcCost(int posx, int posy, int toPosx, int toPosy);
    bool removeFromOpen(int posx, int posy);
    void addToOpen(int posx, int posy);
    int numOfPathfind;
};



class LOSfinder
{
public:
    std::list<point>* calculateVisisble(std::list<point>* retval, int posx, int posy, int posz = 0);
private:
    //bool LOSSquare[sizeHsq * 2 + 1][sizeWsq * 2 + 1];
    void clearLOS();
    std::list<point> worklist;

};

class MapMaster
{
public:
    MapMaster();
    typedef id_ptr_on<CubeTile> SqType;
    std::vector<std::vector<std::vector<SqType> > > squares;

    int GetMapW() const
    {
        return squares.size();
    }
    int GetMapH() const
    {
        return squares[0].size();
    }
    int GetMapD() const
    {
        return squares[0][0].size();
    }

    void ResizeMap(int x, int y, int z);

    void Draw();
    bool isVisible(int posx, int posy, int posz = 0);
   

    template<typename T>
    id_ptr_on<T> getItem(int posx, int posy, int posz = 0)
    {
        return squares[posx][posy][posz]->GetItem<T>();
    }
    
    void splashLiquid(std::list<HashAmount> ha, int posx, int posy, int posz = 0);

    void makeTiles(int x, int y, int z);
    void makeMap();
    void centerFromTo(int nowPosx, int nowPosy, int nowPosz = 0);
    bool isPassable(int posx, int posy, int posz = 0, Dir direct = D_ALL);
    bool fastisPassable(int posx, int posy);
    static void switchDir(int& posx, int& posy, Dir direct, int num = 1, bool back = false);
    static bool checkOutBorder(int posx, int posy);
    //
    static bool checkOutBorder(int posx, int posy, Dir direct);
    //pathfinding

    size_t ms_last_draw;
    bool canDraw();
    
    id_ptr_on<IOnMapObject> click(int x, int y);
    
    int numOfPathfind;
    CPathFinder pathf;
    LOSfinder losf;
    void checkZeroId();
};

MapMaster* GetMapMaster();
void SetMapMaster(MapMaster* map_master);