#ifndef MAPCLASS_H
#define MAPCLASS_H

#include "Mob.h"
#include <stdlib.h>
#include "SDL_thread.h"

struct point
{
    int posx;
    int posy;
};

struct square
{
    bool inOpenList;
    bool inCloseList;
    int cost;
    int realcost;
    int posx;
    int posy;
    square* before;
};

struct pathMessage
{
    int begposx;
    int begposy;
    int toposx;
    int toposy;
    id_ptr id;
};
class CPathFinder
{
public:
    std::list<Dir> calculatePath(int fromPosx, int fromPosy, int toPosx, int toPosy, bool inputlevel);
    MapMaster* map;
private:
    std::list<square*> openList;
    square squares[sizeHmap][sizeWmap];
    std::list<pathMessage> pathListMessage;
    void clearPathfinding();
    void addNear(int posx, int posy, int toPosx, int toPosy);
    int calcCost(int posx, int posy, int toPosx, int toPosy);
    bool removeFromOpen(int posx, int posy);
    void addToOpen(int posx, int posy);
    int numOfPathfind;
    bool level;
};



class LOSfinder
{
public:
    std::list<point>* calculateVisisble(std::list<point>* retval, int posx, int posy, bool level);
    MapMaster* map;
private:
    //bool LOSSquare[sizeHsq * 2 + 1][sizeWsq * 2 + 1];
    void clearLOS();
    std::list<point> worklist;

};

struct idpoint
{
    id_ptr id;
    int posx;
    int posy;
    int find_hash;
};

class MapMaster
{
public:
    MapMaster();
    int loManager;
    ASprClass aSpr;
    Manager* mobi;
    typedef std::list<id_ptr_on<IOnMapItem>> SqType;
    SqType squares[sizeHmap][sizeWmap];
    Screen* screen;
    // Sync stuff

    void Draw();
    bool isVisible(int posx, int posy, bool level);
   

    template<typename T>
    SqType::iterator getItem(int posx, int posy)
    {
        SqType::iterator retval;
        for(retval = squares[posx][posy].begin(); retval != squares[posx][posy].end(); ++retval)
        {
            if(castTo<T>(**retval) != nullptr)
                break;
        }
        return retval;
    }

    template<typename T>
    SqType::iterator getItemOnly(int posx, int posy)
    {
        SqType::iterator retval;
        for(retval = squares[posx][posy].begin(); retval != squares[posx][posy].end(); ++retval)
        {
            if((*retval)->RT_ITEM() == T::RT_ITEM_S())
                break;
        }
        return retval;
    }
    
    void splashLiquid(std::list<HashAmount> ha, int posx, int posy);

    void makeMap();
    void addItemOnMap(id_ptr_on<IOnMapItem> pushval, bool correct_x_y);
    void centerFromTo(int nowPosx, int nowPosy);
    bool isPassable(int posx, int posy, bool level);
    bool fastisPassable(int posx, int posy);
    static void switchDir(int& posx, int& posy, Dir direct, int num = 1, bool back = false);
    static bool checkOutBorder(int posx, int posy);
    //
    static bool checkOutBorder(int posx, int posy, Dir direct);
    //pathfinding

    size_t ms_last_draw;
    bool canDraw();
    
    id_ptr_on<IOnMapItem> click(int x, int y);
    
    int numOfPathfind;
    CPathFinder pathf;
    LOSfinder losf;
    void checkZeroId();
};

#endif