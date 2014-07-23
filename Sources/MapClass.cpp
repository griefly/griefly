#include <math.h>
#include <sstream>
#include <iostream>
#include <assert.h>
#include <hash_map>

#include "MapClass.h"
#include "MainInt.h"
#include "TileInt.h"
#include "Mob.h"
#include "mob_position.h"
#include "ItemFabric.h"
#include "visible_points.h"
#include "SpaceTurf.h"
#include "MetalWall.h"
#include "helpers.h"
#include "Floor.h"
#include "Door.h"
#include "CSmallItem.h"
#include "Grille.h"
#include "Glass.h"
#include "Item.h"
#include "Shard.h"
#include "Weldingtool.h"

void MapMaster::Draw()
{
    if(!canDraw())
        return;
    if(!GetVisible()) 
        return;
    glClear(GL_COLOR_BUFFER_BIT);
    int z_level_m = mob_position::get_mob_z();
    for (int z_level = 0; z_level < GetMapD(); z_level++) 
    {
        for(int i = 0; i < MAX_LEVEL; ++i)
        {
            auto it2 = GetVisible()->begin();
            while(it2 != GetVisible()->end())
            {   
                if(checkOutBorder(it2->posx, it2->posy))
                {
                    if (it2->posz == z_level)
                    {
                        squares[it2->posx][it2->posy][it2->posz]->ForEach([&](id_ptr_on<IOnMapBase> item)
                        {
                            auto item_n = castTo<IOnMapObject>(item.ret_item());
                            if (item_n->v_level == i)
                                item_n->processImage(z_level < z_level_m ? TOP : SAME);//screen
                        });
                        auto trf = squares[it2->posx][it2->posy][it2->posz]->GetTurf();
                        if (trf.valid() && trf->v_level == i)
                            trf->processImage(z_level < z_level_m ? TOP : SAME);
                    }
                }
                ++it2;
            }
        } 
        auto it2 = GetVisible()->begin();
        while(it2 != GetVisible()->end())
        {   
            if(checkOutBorder(it2->posx, it2->posy))
            {
                if (it2->posz == z_level)
                {
                    squares[it2->posx][it2->posy][it2->posz]->ForEach([&](id_ptr_on<IOnMapBase> item)
                    {
                        auto item_n = castTo<IOnMapObject>(item.ret_item());
                        if (item_n->v_level >= MAX_LEVEL)
                            item_n->processImage(z_level < z_level_m ? TOP : SAME);//screen
                    });
                    auto trf = squares[it2->posx][it2->posy][it2->posz]->GetTurf();
                    if (trf.valid() && trf->v_level >= MAX_LEVEL)
                        trf->processImage(z_level < z_level_m ? TOP : SAME);
                }
            }
            ++it2;
        }
    }
};

void MapMaster::makeTiles(int new_map_x, int new_map_y, int new_map_z)
{
    ResizeMap(new_map_x, new_map_y, new_map_z);
    for(int x = 0; x < GetMapW(); x++)
    {
        for(int y = 0; y < GetMapH(); y++) 
        {
            for (int z = 0; z < GetMapD(); z++)
            {
                auto loc = GetItemFabric()->newItem<CubeTile>(CubeTile::T_ITEM_S());
                loc->SetPos(x, y, z);
                squares[x][y][z] = loc;
            }
        }
    }
}

void MapMaster::ResizeMap(int new_map_x, int new_map_y, int new_map_z)
{
    squares.resize(new_map_x);
    for (int x = 0; x < new_map_x; ++x)
    {
        squares[x].resize(new_map_y);
        for (int y = 0; y < new_map_y; ++y)
        {
            squares[x][y].resize(new_map_z);
        }
    }
}

void MapMaster::makeMap()
{
    // End gen
    for(int x = 0; x < GetMapW(); x++)
    {
        for(int y = 0; y < GetMapH(); y++) 
        {     
            // Ge
            //
            bool chk = (rand() % 10 != 1);
            bool bin = (rand() % 2) == 0;
            bool bin2 = (rand() % 2) == 0;
            id_ptr_on<IOnMapObject> loc = 
                GetItemFabric()->newItem<IOnMapObject>(  chk  ?        Space::T_ITEM_S() 
                                                     : bin  ?      MetalWall::T_ITEM_S()
                                                     : bin2 ? ReinforcedWall::T_ITEM_S()
                                                     :                 Floor::T_ITEM_S());

            squares[x][y][0]->SetTurf(loc);
            if (rand() % 30 == 1)
                GetItemFabric()->newItemOnMap<IOnMapObject>(Wrench::T_ITEM_S(), squares[x][y][0]);
            if (rand() % 30 == 1)
                GetItemFabric()->newItemOnMap<IOnMapObject>(Weldingtool::T_ITEM_S(), squares[x][y][0]);
            if (rand() % 24 == 1)
                GetItemFabric()->newItemOnMap<IOnMapObject>(Crowbar::T_ITEM_S(), squares[x][y][0]);
            if (rand() % 14 == 1)
                GetItemFabric()->newItemOnMap<IOnMapObject>(Wirecutters::T_ITEM_S(), squares[x][y][0]);
            if (rand() % 7 == 1)
                GetItemFabric()->newItemOnMap<IOnMapObject>(Screwdriver::T_ITEM_S(), squares[x][y][0]);
            if (rand() % 20 == 1)
                GetItemFabric()->newItemOnMap<IOnMapObject>(ReinforcedFlatGlass::T_ITEM_S(), squares[x][y][0]);
            if (rand() % 9 == 1)
                GetItemFabric()->newItemOnMap<IOnMapObject>(FlatGlass::T_ITEM_S(), squares[x][y][0]);
            if (rand() % 9 == 1)
                GetItemFabric()->newItemOnMap<IOnMapObject>(Grille::T_ITEM_S(), squares[x][y][0]);
            if (rand() % 10 == 1)
                GetItemFabric()->newItemOnMap<IOnMapObject>(Door::T_ITEM_S(), squares[x][y][0]);
            if (rand() % 3 == 1)
                GetItemFabric()->newItemOnMap<IOnMapObject>(Shard::T_ITEM_S(), squares[x][y][0]);//*/
        }
    }
    SYSTEM_STREAM << "End create map\n";
};

MapMaster::MapMaster()
{
    ms_last_draw = 0;
};

bool MapMaster::canDraw()
{
    if (SDL_GetTicks() - ms_last_draw > static_cast<size_t>(1000 / DRAW_MAX + 1))
    {
        ms_last_draw = SDL_GetTicks();
        return true;
    }
    return false;
}

void CPathFinder::clearPathfinding()
{

    squares.resize(GetMapMaster()->GetMapW());
    for(int i = 0; i < GetMapMaster()->GetMapW(); ++i)
    {
        squares[i].resize(GetMapMaster()->GetMapH());
        for(int j = 0; j < GetMapMaster()->GetMapH(); ++j)
        {
            squares[i][j].before = 0;
            squares[i][j].inCloseList = false;
            squares[i][j].inOpenList = false;
            squares[i][j].cost = 0;
            squares[i][j].posx = i;
            squares[i][j].posy = j;
        }
    }
    openList.clear();
};


bool MapMaster::isPassable(int posx, int posy, int posz, Dir direct)
{
    return squares[posx][posy][posz]->IsPassable(direct);
};

void MapMaster::switchDir(int& posx, int& posy, Dir direct, int num, bool back)//TODO: Remove back
{
    if(!back)
    {
        switch(direct)
        {
        case D_UP:
            posy -= num;
            return;
        case D_DOWN:
            posy += num;
            return;
        case D_LEFT:
            posx -= num;
            return;
        case D_RIGHT:
            posx += num;
            return;
        }
    }
    else
    {
        switch(direct)
        {
        case D_UP:
            posy += num;
            return;
        case D_DOWN:
            posy -= num;
            return;
        case D_LEFT:
            posx += num;
            return;
        case D_RIGHT:
            posx -= num;
            return;
        }
    }
};

bool MapMaster::checkOutBorder(int posx, int posy)
{
    if(    posy < 0 || posy > (GetMapMaster()->GetMapH() - 1) 
        || posx < 0 || posx > (GetMapMaster()->GetMapW() - 1)) 
        return false;
    return true;
};

bool MapMaster::checkOutBorder(int posx, int posy, Dir direct)
{
    if(    (direct == D_UP    && posy <= 0) 
        || (direct == D_DOWN  && posy >= (GetMapMaster()->GetMapH() - 1)) 
        || (direct == D_LEFT  && posx <= 0) 
        || (direct == D_RIGHT && posx >= (GetMapMaster()->GetMapW() - 1))) 
        return false;
    return true;
};

bool MapMaster::isVisible(int posx, int posy, int posz)
{
    if(!checkOutBorder(posx, posy/*TODO: posz*/))
        return false;
    return squares[posx][posy][posz]->IsTransparent();
};

void MapMaster::splashLiquid(std::list<HashAmount> ha, int posx, int posy, int posz)
{
    // TODO:
    /*std::list<HashAmount>* loc = &ha;
    auto it = squares[posx][posy][].end();
    if(it == squares[posx][posy].begin())
        return;
    do
    {
        --it;
        std::list<HashAmount> ha = (*it)->insertLiquid(*loc);
    }
    while(ha.size() && it != squares[posx][posy].begin());*/
}

id_ptr_on<IOnMapObject> MapMaster::click(int x, int y)
{
    if(!GetVisible()) 
        return 0;

    x = static_cast<int>(static_cast<float>(x) * (static_cast<float>(sizeW + guiShift) / static_cast<float>(GetScreen()->w())));;
    y = static_cast<int>(static_cast<float>(y) * (static_cast<float>(sizeH) / static_cast<float>(GetScreen()->h())));

    id_ptr_on<IOnMapObject> retval = 0;

    int z_level_m = mob_position::get_mob_z();
    for (int z = z_level_m; z >= 0; --z)
    {
        auto it2 = GetVisible()->begin();  
        while(it2 != GetVisible()->end())
        {
            if (it2->posz == z)
            {
                squares[it2->posx][it2->posy][it2->posz]->ForEach([&](id_ptr_on<IOnMapBase> item_h)
                {
                    auto item = castTo<IOnMapObject>(item_h.ret_item());
                    if (retval.ret_id() == 0)
                        if(item->v_level >= MAX_LEVEL)
                            if(!item->IsTransp(
                                x - (item->GetDrawX() + mob_position::get_shift_x()),
                                y - (item->GetDrawY() + mob_position::get_shift_y())))
                                retval = item_h;
                });
                auto trf = squares[it2->posx][it2->posy][it2->posz]->GetTurf();
                if (trf.valid())
                    if (retval.ret_id() == 0)
                        if(trf->v_level >= MAX_LEVEL)
                            if(!trf->IsTransp(
                                x - (trf->GetDrawX() + mob_position::get_shift_x()),
                                y - (trf->GetDrawY() + mob_position::get_shift_y())))
                                retval = trf;
            }
            if (retval.valid())
                return retval;
            ++it2;
        }
        it2 = GetVisible()->begin();  
        for(int i = MAX_LEVEL - 1; i >= 0; --i)
        {
            auto it2 = GetVisible()->begin();  
            while(it2 != GetVisible()->end())
            {
                if (it2->posz == z)
                {
                    squares[it2->posx][it2->posy][it2->posz]->ForEach([&](id_ptr_on<IOnMapBase> item_h)
                    {
                        auto item = castTo<IOnMapObject>(item_h.ret_item());
                        if (retval.ret_id() == 0)
                            if(item->v_level == i)
                                if(!item->IsTransp(
                                    x - (item->GetDrawX() + mob_position::get_shift_x()),
                                    y - (item->GetDrawY() + mob_position::get_shift_y())))
                                    retval = item_h;
                    });
                    auto trf = squares[it2->posx][it2->posy][it2->posz]->GetTurf();
                    if (trf.valid())
                        if (retval.ret_id() == 0)
                            if(trf->v_level == i)
                                if(!trf->IsTransp(
                                    x - (trf->GetDrawX() + mob_position::get_shift_x()),
                                    y - (trf->GetDrawY() + mob_position::get_shift_y())))
                                    retval = trf;
                }
                if (retval.ret_id())
                    return retval;
                ++it2;
            }
        }
    }
    return 0;
}

MapMaster* map_master_ = 0;
MapMaster* GetMapMaster()
{
    return map_master_;
}

void SetMapMaster(MapMaster* map_master)
{
    map_master_ = map_master;
}

std::list<Dir> CPathFinder::calculatePath(int fromPosx, int fromPosy, int toPosx, int toPosy, int toPosz)
{
    int begTime = SDL_GetTicks();
    ++numOfPathfind;
    clearPathfinding();
    std::list<Dir> path;
    int locposx = fromPosx;
    int locposy = fromPosy;
    squares[locposx][locposy].realcost = 0;
    squares[locposx][locposy].cost = calcCost(locposx, locposy, toPosx, toPosy);
    squares[locposx][locposy].inOpenList = true;
    addToOpen(locposx, locposy);
    int i = 0;
    while(locposx != toPosx || locposy != toPosy)
    {
        i++;

        addNear(locposx, locposy, toPosx, toPosy);
        squares[locposx][locposy].inOpenList = false;
        squares[locposx][locposy].inCloseList = true;
        openList.pop_front();
        
        if(openList.begin() == openList.end()) 
            return path;
        locposx = (*openList.begin())->posx;
        locposy = (*openList.begin())->posy;
    }
    //printf("\n%d - number seen squares\n", i);
    square* sq = &squares[toPosx][toPosy];
    while(sq != &squares[fromPosx][fromPosy])
    {
        if(sq->posx < sq->before->posx) 
            path.push_front(D_LEFT);
        else if(sq->posx > sq->before->posx) 
            path.push_front(D_RIGHT);
        else if(sq->posy < sq->before->posy) 
            path.push_front(D_UP);
        else 
            path.push_front(D_DOWN);
        sq = sq->before;
    }
    //SYSTEM_STREAM << SDL_GetTicks() - begTime << " Pathspeed\n";
    return path;
};


void CPathFinder::addNear(int posx, int posy, int toPosx, int toPosy)
{
    //printf("%d %d huj\n", posx, posy);

    if(!(squares[posx + 1][posy].inCloseList) && GetMapMaster()->isPassable(posx + 1, posy))
    {
        if(squares[posx + 1][posy].inOpenList)
        {
            if(squares[posx][posy].realcost + 1 < squares[posx + 1][posy].realcost)
            {
                squares[posx + 1][posy].realcost = squares[posx][posy].realcost + 1;
                squares[posx + 1][posy].cost = calcCost(posx, posy, toPosx, toPosy);
                squares[posx + 1][posy].before = &squares[posx][posy];
                removeFromOpen(posx + 1, posy);
                addToOpen(posx + 1, posy);
            }
        }
        else
        {
            squares[posx + 1][posy].realcost = squares[posx][posy].realcost + 1;
            squares[posx + 1][posy].cost = calcCost(posx, posy, toPosx, toPosy);
            squares[posx + 1][posy].before = &squares[posx][posy];
            squares[posx + 1][posy].inOpenList = true;
            addToOpen(posx + 1, posy);
        }
    }
    //printf("Wtf!\n");
    if(!squares[posx - 1][posy].inCloseList && GetMapMaster()->isPassable(posx - 1, posy))
    {
        if(squares[posx - 1][posy].inOpenList)
        {
            if(squares[posx][posy].realcost + 1 < squares[posx - 1][posy].realcost)
            {
                squares[posx - 1][posy].realcost = squares[posx][posy].realcost + 1;
                squares[posx - 1][posy].cost = calcCost(posx, posy, toPosx, toPosy);
                squares[posx - 1][posy].before = &squares[posx][posy];
                removeFromOpen(posx - 1, posy);
                addToOpen(posx - 1, posy);
            }
        }
        else
        {
            squares[posx - 1][posy].realcost = squares[posx][posy].realcost + 1;
            squares[posx - 1][posy].cost = calcCost(posx, posy, toPosx, toPosy);
            squares[posx - 1][posy].before = &squares[posx][posy];
            squares[posx - 1][posy].inOpenList = true;
            addToOpen(posx - 1, posy);
        }
    }
    if(!squares[posx][posy + 1].inCloseList && GetMapMaster()->isPassable(posx, posy + 1))
    {
        if(squares[posx][posy + 1].inOpenList)
        {
            if(squares[posx][posy].realcost + 1 < squares[posx][posy + 1].realcost)
            {
                squares[posx][posy + 1].realcost = squares[posx][posy].realcost + 1;
                squares[posx][posy + 1].cost = calcCost(posx, posy, toPosx, toPosy);
                squares[posx][posy + 1].before = &squares[posx][posy];
                removeFromOpen(posx, posy + 1);
                addToOpen(posx, posy + 1);
            }
        }
        else
        {
            squares[posx][posy + 1].realcost = squares[posx][posy].realcost + 1;
            squares[posx][posy + 1].cost = calcCost(posx, posy, toPosx, toPosy);
            squares[posx][posy + 1].before = &squares[posx][posy];
            squares[posx][posy + 1].inOpenList = true;
            addToOpen(posx, posy + 1);
        }
    }
    if(!squares[posx][posy - 1].inCloseList && GetMapMaster()->isPassable(posx, posy - 1))
    {
        if(squares[posx][posy - 1].inOpenList)
        {
            if(squares[posx][posy].realcost + 1 < squares[posx][posy - 1].realcost)
            {
                squares[posx][posy - 1].realcost = squares[posx][posy].realcost + 1;
                squares[posx][posy - 1].cost = calcCost(posx, posy, toPosx, toPosy);
                squares[posx][posy - 1].before = &squares[posx][posy];
                removeFromOpen(posx, posy - 1);
                addToOpen(posx, posy - 1);
            }
        }
        else
        {
            squares[posx][posy - 1].realcost = squares[posx][posy].realcost + 1;
            squares[posx][posy - 1].cost = calcCost(posx, posy, toPosx, toPosy);
            squares[posx][posy - 1].before = &squares[posx][posy];
            squares[posx][posy - 1].inOpenList = true;
            addToOpen(posx, posy - 1);
        }
    }
};

int CPathFinder::calcCost(int posx, int posy, int toPosx, int toPosy)
{
    return squares[posx][posy].realcost + abs(posx - toPosx) + abs(posy - toPosy);
};

bool CPathFinder::removeFromOpen(int posx, int posy)
{
    auto itr = openList.begin();
    while(itr != openList.end())
    {
        if(*itr == &squares[posx][posy]) break;
        itr++;
    }
    if(itr == openList.end()) return false;
    openList.erase(itr);
    return true;
};

void CPathFinder::addToOpen(int posx, int posy)
{
    auto itr = openList.begin();
    while(itr != openList.end())
    {
        if((*itr)->cost > squares[posx][posy].cost)
        {
            openList.insert(itr, &squares[posx][posy]);
            return;
        }
        ++itr;
    }   
    openList.push_back(&squares[posx][posy]);
};

std::list<point>* LOSfinder::calculateVisisble(std::list<point>* retlist, int posx, int posy, int posz)
{
    //auto retlist = new std::list<point>;
    clearLOS();
    //for(int level = 0; level < 
    point p = {posx, posy, posz};
    retlist->push_back(p);

    p.posx = posx + 1;
    p.posy = posy;
    if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
        worklist.push_back(p);

    p.posx = posx + 1;
    p.posy = posy;
    if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
        retlist->push_back(p);

    p.posx = posx + 1;
    p.posy = posy + 1;
    if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
        worklist.push_back(p);

    p.posx = posx + 1;
    p.posy = posy + 1;
    if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
        retlist->push_back(p);

    p.posx = posx;
    p.posy = posy + 1;
    if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
        worklist.push_back(p);

    p.posx = posx;
    p.posy = posy + 1;
    if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
        retlist->push_back(p);

    p.posx = posx - 1;
    p.posy = posy + 1;
    if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
        worklist.push_back(p);

    p.posx = posx - 1;
    p.posy = posy + 1;
    if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
        retlist->push_back(p);

    p.posx = posx - 1;
    p.posy = posy;
    if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
        worklist.push_back(p);

    p.posx = posx - 1;
    p.posy = posy;
    if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
        retlist->push_back(p);
    
    p.posx = posx - 1;
    p.posy = posy - 1;
    if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
        worklist.push_back(p);

    p.posx = posx - 1;
    p.posy = posy - 1;
    if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
        retlist->push_back(p);

    p.posx = posx;
    p.posy = posy - 1;
    if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
        worklist.push_back(p);

    p.posx = posx;
    p.posy = posy - 1;
    if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
        retlist->push_back(p);

    p.posx = posx + 1;
    p.posy = posy - 1;
    if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
        worklist.push_back(p);

    p.posx = posx + 1;
    p.posy = posy - 1;
    if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
        retlist->push_back(p);

    auto itr = worklist.begin();
    while(itr != worklist.end())
    {
        if(GetMapMaster()->isVisible(itr->posx, itr->posy, itr->posz) 
            && itr->posx != posx - sizeHsq && itr->posx != posx + sizeHsq
            && itr->posy != posy - sizeWsq && itr->posy != posy + sizeWsq
            && 
            helpers::check_borders(&itr->posx, &itr->posy, &itr->posz))
            if(abs(itr->posx - posx) > abs(itr->posy - posy)) 
            if(itr->posx > posx)
            {
                p.posx = itr->posx + 1;
                p.posy = itr->posy;
                if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
                    worklist.push_back(p);
                p.posx = itr->posx + 1;
                p.posy = itr->posy;
                if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
                    retlist->push_back(p);
            }
            else 
            {
                p.posx = itr->posx - 1;
                p.posy = itr->posy;
                if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
                    worklist.push_back(p);
                p.posx = itr->posx - 1;
                p.posy = itr->posy;
                if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
                    retlist->push_back(p);
            }
            else if(abs(itr->posx - posx) < abs(itr->posy - posy)) 
            if(itr->posy > posy) 
            {
                p.posx = itr->posx;
                p.posy = itr->posy + 1;
                if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
                    worklist.push_back(p);
                p.posx = itr->posx;
                p.posy = itr->posy + 1;
                if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
                    retlist->push_back(p);
            }
            else 
            {
                p.posx = itr->posx;
                p.posy = itr->posy - 1;
                if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
                    worklist.push_back(p);
                p.posx = itr->posx;
                p.posy = itr->posy - 1;
                if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
                    retlist->push_back(p);
            }
            else
            if(itr->posx > posx)
            {
                if(itr->posy > posy)
                {
                p.posx = itr->posx + 1;
                p.posy = itr->posy + 1;
                if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
                    worklist.push_back(p);
                p.posx = itr->posx + 1;
                p.posy = itr->posy + 1;
                if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
                    retlist->push_back(p);
                p.posx = itr->posx + 1;
                p.posy = itr->posy;
                if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
                    worklist.push_back(p);
                p.posx = itr->posx + 1;
                p.posy = itr->posy;
                if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
                    retlist->push_back(p);
                p.posx = itr->posx;
                p.posy = itr->posy + 1;
                if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
                    worklist.push_back(p);
                p.posx = itr->posx;
                p.posy = itr->posy + 1;
                if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
                    retlist->push_back(p);
                }
                else
                {
                p.posx = itr->posx + 1;
                p.posy = itr->posy - 1;
                if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
                    worklist.push_back(p);
                p.posx = itr->posx + 1;
                p.posy = itr->posy - 1;
                if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
                    retlist->push_back(p);
                p.posx = itr->posx + 1;
                p.posy = itr->posy;
                if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
                    worklist.push_back(p);
                p.posx = itr->posx + 1;
                p.posy = itr->posy;
                if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
                    retlist->push_back(p);
                p.posx = itr->posx;
                p.posy = itr->posy - 1;
                if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
                    worklist.push_back(p);
                p.posx = itr->posx;
                p.posy = itr->posy - 1;
                if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
                    retlist->push_back(p);
                }

            }
            else
            {
                if(itr->posy > posy)
                {
                p.posx = itr->posx - 1;
                p.posy = itr->posy + 1;
                if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
                    worklist.push_back(p);
                p.posx = itr->posx - 1;
                p.posy = itr->posy + 1;
                if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
                    retlist->push_back(p);
                p.posx = itr->posx;
                p.posy = itr->posy + 1;
                if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
                    worklist.push_back(p);
                p.posx = itr->posx;
                p.posy = itr->posy + 1;
                if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
                    retlist->push_back(p);
                p.posx = itr->posx - 1;
                p.posy = itr->posy;
                if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
                    worklist.push_back(p);
                p.posx = itr->posx - 1;
                p.posy = itr->posy;
                if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
                    retlist->push_back(p);
                }
                else
                {
                p.posx = itr->posx - 1;
                p.posy = itr->posy - 1;
                if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
                    worklist.push_back(p);
                p.posx = itr->posx - 1;
                p.posy = itr->posy - 1;
                if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
                    retlist->push_back(p);
                p.posx = itr->posx - 1;
                p.posy = itr->posy;
                if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
                    worklist.push_back(p);
                p.posx = itr->posx - 1;
                p.posy = itr->posy;
                if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
                    retlist->push_back(p);
                p.posx = itr->posx;
                p.posy = itr->posy - 1;
                if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
                    worklist.push_back(p);
                p.posx = itr->posx;
                p.posy = itr->posy - 1;
                if (helpers::check_borders(&p.posx, &p.posy, &p.posz))
                    retlist->push_back(p);
                }
            }

        worklist.erase(itr++);
    };
    if (retlist->begin()->posz == 0)
        return retlist;
    
    std::list<point> z_list;
    for (auto it = retlist->begin(); it != retlist->end(); ++it)
    {
        point p = *it;
        p.posz -= 1;
        z_list.push_back(p);
    }
    retlist->splice(retlist->begin(), z_list);

    return retlist;
};

void LOSfinder::clearLOS()
{
    worklist.clear();
};