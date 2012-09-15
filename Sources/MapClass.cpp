#include "MapClass.h"
#include "MainInt.h"

#include <math.h>
#include <sstream>
#include <iostream>
#include <assert.h>

void MapMaster::Draw()
{
    if(!canDraw())
        return;
    if(!mobi->visiblePoint) 
        return;
    glClear(GL_COLOR_BUFFER_BIT);
    auto it2 = mobi->visiblePoint->begin();  
    for(int i = 0; i < MAX_LEVEL; ++i)
    {
        auto it2 = mobi->visiblePoint->begin();  
        while(it2 != mobi->visiblePoint->end())
        {
            auto itr = squares[it2->posx][it2->posy].begin();
            while(itr != squares[it2->posx][it2->posy].end())
            {
                if((*itr)->v_level == i)
                    (*itr)->processImage(nullptr/*screen*/);   
                ++itr;
            }
            ++it2;
        }
    }
    it2 = mobi->visiblePoint->begin();  
    while(it2 != mobi->visiblePoint->end())
    {
        auto itr = squares[it2->posx][it2->posy].begin();
        while(itr != squares[it2->posx][it2->posy].end())
        {
            if((*itr)->v_level >= MAX_LEVEL)
                (*itr)->processImage(nullptr/*screen*/);   
            ++itr;
        }
        ++it2;
    }
};

void MapMaster::makeMap()
{
    for(int x = 0; x < sizeWmap; x++)
    {
        for(int y = 0; y < sizeHmap; y++) 
        {     

            //*
            id_ptr_on<IOnMapItem> loc = IMainItem::fabric->newItemOnMap<IOnMapItem>(hash((rand() % 10 != 1) ? "ground" : "ground"), x, y);
            loc->imageStateH = 0;
            loc->imageStateW = rand() % 4;
            loc->level = 0;//*/
            
            if(rand() % 29 == 1 || x == 0 || y == 0 || x == sizeWmap - 1 || y == sizeHmap - 1)
                IMainItem::fabric->newItemOnMap<IOnMapItem>(hash("testmob"), x, y)->level = 1;
            if(rand() % 60 == 1 && x != 0 && y != 0 && x != sizeWmap - 1 && y != sizeHmap - 1)
                IMainItem::fabric->newItemOnMap<IOnMapItem>(hash("kivsjak"), x, y);
            if(rand() % 3 == 1 && x != 0 && y != 0 && x != sizeWmap - 1 && y != sizeHmap - 1)
                IMainItem::fabric->newItemOnMap<IOnMapItem>(hash("weed"), x, y);//*/
        }
    }
    SYSTEM_STREAM << "End create map\n";
};

void MapMaster::centerFromTo(int beginPosx, int beginPosy, int nowPosx, int nowPosy)
{
    int xshift = (beginPosx - nowPosx) * TITLE_SIZE;
    int yshift = (beginPosy - nowPosy) * TITLE_SIZE;
    for(int x = 0; x < sizeWmap; x++)
    {
        for(int y = 0; y < sizeHmap; y++) 
        {
            auto itr = squares[x][y].begin();
            while(itr != squares[x][y].end())
            {
                (*itr)->x += xshift;
                (*itr++)->y += yshift;
            }
        }
    }
};

MapMaster::MapMaster()
{
    ms_last_draw = 0;
    pathf.map = this;
    losf.map = this;  
};

bool MapMaster::canDraw()
{
    if (SDL_GetTicks() - ms_last_draw > (1000 / DRAW_MAX + 1))
    {
        ms_last_draw = SDL_GetTicks();
        return true;
    }
    return false;
}

void CPathFinder::clearPathfinding()
{
    for(int i = 0; i < sizeHmap; ++i)
    {
        for(int j = 0; j < sizeWmap; ++j)
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


bool MapMaster::isPassable(int posx, int posy, bool level)
{
    auto itr = squares[posx][posy].begin();
    while(itr != squares[posx][posy].end())
    {
        if((!(*itr)->passable && ((*itr)->level == level))) return false;
        ++itr;
    }
    return true;
};

//Unused Delete it
bool MapMaster::fastisPassable(int posx, int posy)
{
    if(posy <= 0 || posy >= (sizeHmap - 1) || posx <= 0 || posx >= sizeWmap - 1) return false;
    return true;
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
    if(posy < 0 || posy > (sizeHmap - 1) || posx < 0 || posx > sizeWmap - 1) return false;
    return true;
};

bool MapMaster::checkOutBorder(int posx, int posy, Dir direct)
{
    if((direct == D_UP && posy <= 0) || (direct == D_DOWN && posy >= (sizeHmap - 1)) || (direct == D_LEFT && posx <= 0) || (direct == D_RIGHT && posx >= (sizeWmap - 1))) return false;
    return true;
};

bool MapMaster::isVisible(int posx, int posy, bool level)
{
    if(!checkOutBorder(posx, posy)) return false;
    auto itr = squares[posx][posy].begin();
    while(itr != squares[posx][posy].end())
    {
        if((!(*itr)->transparent && ((*itr)->level == level))) return false;
        ++itr;
    }
    return true;
};

void MapMaster::addItemOnMap(id_ptr_on<IOnMapItem> pushval)
{
    // TODO: сортировка в правильном порядке, и вообще потом
    int posx = pushval->posx, posy = pushval->posy;
    auto itr = squares[posx][posy].begin();
    while(itr != squares[posx][posy].end())
    {
        if((*itr)->v_level > pushval->v_level)
            break;
        ++itr;
    }
    SqType::const_iterator locit = itr;
    squares[posx][posy].insert(locit, pushval);
    //pushval->x = 
}

void MapMaster::splashLiquid(std::list<HashAmount> ha, int posx, int posy)
{
    std::list<HashAmount>* loc = &ha;
    auto it = squares[posx][posy].end();
    if(it == squares[posx][posy].begin())
        return;
    do
    {
        --it;
        SYSTEM_STREAM << "Begin insert liquid\n";
        std::list<HashAmount> ha = (*it)->insertLiquid(*loc);
        SYSTEM_STREAM << "End insert liquid\n";
    }
    while(ha.size() && it != squares[posx][posy].begin());
}

id_ptr_on<IOnMapItem> MapMaster::click(int x, int y)
{
    if(!mobi->visiblePoint) 
        return 0;
    auto it2 = mobi->visiblePoint->begin();  
    while(it2 != mobi->visiblePoint->end())
    {
        auto itr = squares[it2->posx][it2->posy].begin();
        while(itr != squares[it2->posx][it2->posy].end())
        {
            if((*itr)->v_level >= MAX_LEVEL)
                if(!(*itr)->IsTransp(x - (*itr)->x, y - (*itr)->y))
                    return *itr;
            ++itr;
        }
        ++it2;
    }
    it2 = mobi->visiblePoint->begin();  
    for(int i = MAX_LEVEL - 1; i >= 0; --i)
    {
        auto it2 = mobi->visiblePoint->begin();  
        while(it2 != mobi->visiblePoint->end())
        {
            auto itr = squares[it2->posx][it2->posy].begin();
            while(itr != squares[it2->posx][it2->posy].end())
            {
                if((*itr)->v_level == i)
                    if(!(*itr)->IsTransp(x - (*itr)->x, y - (*itr)->y))
                        return *itr;
                ++itr;
            }
            ++it2;
        }
    }
    return 0;
}

std::list<Dir> CPathFinder::calculatePath(int fromPosx, int fromPosy, int toPosx, int toPosy, bool inputlevel)
{
    int begTime = SDL_GetTicks();
    level = inputlevel;
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
        {
            //SYSTEM_STREAM << SDL_GetTicks() - begTime << " Pathspeed\n";
            return path;
        }
        locposx = (*openList.begin())->posx;
        locposy = (*openList.begin())->posy;
    }
    //printf("\n%d - number seen squares\n", i);
    square* sq = &squares[toPosx][toPosy];
    while(sq != &squares[fromPosx][fromPosy])
    {
        if(sq->posx < sq->before->posx) path.push_front(D_LEFT);
        else if(sq->posx > sq->before->posx) path.push_front(D_RIGHT);
        else if(sq->posy < sq->before->posy) path.push_front(D_UP);
        else path.push_front(D_DOWN);
        sq = sq->before;
    }
    //SYSTEM_STREAM << SDL_GetTicks() - begTime << " Pathspeed\n";
    return path;
};


void CPathFinder::addNear(int posx, int posy, int toPosx, int toPosy)
{
    //printf("%d %d huj\n", posx, posy);

    if(!(squares[posx + 1][posy].inCloseList) && map->isPassable(posx + 1, posy, level))
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
    if(!squares[posx - 1][posy].inCloseList && map->isPassable(posx - 1, posy, level))
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
    if(!squares[posx][posy + 1].inCloseList && map->isPassable(posx, posy + 1, level))
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
    if(!squares[posx][posy - 1].inCloseList && map->isPassable(posx, posy - 1, level))
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

std::list<point>* LOSfinder::calculateVisisble(std::list<point>* retlist, int posx, int posy, bool level)
{
    //auto retlist = new std::list<point>;
    clearLOS();
    //for(int level = 0; level < 
    point p = {sizeHsq + posx - sizeHsq, sizeWsq + posy - sizeWsq};
    retlist->push_back(p);

    p.posx = sizeHsq + 1;
    p.posy = sizeWsq;
    worklist.push_back(p);

    p.posx = sizeHsq + 1 + posx - sizeHsq;
    p.posy = sizeWsq + posy - sizeWsq;
    retlist->push_back(p);

    p.posx = sizeHsq + 1;
    p.posy = sizeWsq + 1;
    worklist.push_back(p);

    p.posx = sizeHsq + 1 + posx - sizeHsq;
    p.posy = sizeWsq + 1 + posy - sizeWsq;
    retlist->push_back(p);

    p.posx = sizeHsq;
    p.posy = sizeWsq + 1;
    worklist.push_back(p);

    p.posx = sizeHsq + posx - sizeHsq;
    p.posy = sizeWsq + 1 + posy - sizeWsq;
    retlist->push_back(p);

    p.posx = sizeHsq - 1;
    p.posy = sizeWsq + 1;
    worklist.push_back(p);

    p.posx = sizeHsq - 1 + posx - sizeHsq;
    p.posy = sizeWsq + 1 + posy - sizeWsq;
    retlist->push_back(p);

    p.posx = sizeHsq - 1;
    p.posy = sizeWsq;
    worklist.push_back(p);

    p.posx = sizeHsq - 1 + posx - sizeHsq;
    p.posy = sizeWsq + posy - sizeWsq;
    retlist->push_back(p);
    
    p.posx = sizeHsq - 1;
    p.posy = sizeWsq - 1;
    worklist.push_back(p);

    p.posx = sizeHsq - 1 + posx - sizeHsq;
    p.posy = sizeWsq - 1 + posy - sizeWsq;    
    retlist->push_back(p);

    p.posx = sizeHsq;
    p.posy = sizeWsq - 1;
    worklist.push_back(p);

    p.posx = sizeHsq + posx - sizeHsq;
    p.posy = sizeWsq - 1 + posy - sizeWsq;  
    retlist->push_back(p);

    p.posx = sizeHsq + 1;
    p.posy = sizeWsq - 1;
    worklist.push_back(p);

    p.posx = sizeHsq + 1 + posx - sizeHsq;
    p.posy = sizeWsq - 1 + posy - sizeWsq; 
    retlist->push_back(p);
    auto itr = worklist.begin();
    while(itr != worklist.end())
    {
        if(map->isVisible(itr->posx + posx - sizeHsq, itr->posy + posy - sizeWsq, level) && itr->posx != 0 && itr->posx != sizeHsq * 2 && itr->posy != sizeWsq * 2 && itr->posy != 0
            && !(itr->posy + posy - sizeWsq <= 0 || itr->posy + posy - sizeWsq >= (sizeHmap - 1) || itr->posx + posx - sizeHsq <= 0 || itr->posx + posx - sizeHsq >= sizeWmap - 1))
            if(abs(itr->posx - sizeHsq) > abs(itr->posy - sizeWsq)) 
            if(itr->posx > sizeHsq)
            {
                p.posx = itr->posx + 1;
                p.posy = itr->posy;
                worklist.push_back(p);
                p.posx = itr->posx + 1 + posx - sizeHsq;
                p.posy = itr->posy + posy - sizeWsq;
                retlist->push_back(p);
            }
            else 
            {
                p.posx = itr->posx - 1;
                p.posy = itr->posy;
                worklist.push_back(p);
                p.posx = itr->posx - 1 + posx - sizeHsq;
                p.posy = itr->posy + posy - sizeWsq;
                retlist->push_back(p);
            }
            else if(abs(itr->posx - sizeHsq) < abs(itr->posy - sizeWsq)) 
            if(itr->posy > sizeWsq) 
            {
                p.posx = itr->posx;
                p.posy = itr->posy + 1;
                worklist.push_back(p);
                p.posx = itr->posx + posx - sizeHsq;
                p.posy = itr->posy + 1 + posy - sizeWsq;
                retlist->push_back(p);
            }
            else 
            {
                p.posx = itr->posx;
                p.posy = itr->posy - 1;
                worklist.push_back(p);
                p.posx = itr->posx + posx - sizeHsq;
                p.posy = itr->posy - 1 + posy - sizeWsq;
                retlist->push_back(p);
            }
            else
            if(itr->posx > sizeHsq)
            {
                if(itr->posy > sizeWsq)
                {
                p.posx = itr->posx + 1;
                p.posy = itr->posy + 1;
                worklist.push_back(p);
                p.posx = itr->posx + 1 + posx - sizeHsq;
                p.posy = itr->posy + 1 + posy - sizeWsq;
                retlist->push_back(p);
                p.posx = itr->posx + 1;
                p.posy = itr->posy;
                worklist.push_back(p);
                p.posx = itr->posx + 1 + posx - sizeHsq;
                p.posy = itr->posy + posy - sizeWsq;
                retlist->push_back(p);
                p.posx = itr->posx;
                p.posy = itr->posy + 1;
                worklist.push_back(p);
                p.posx = itr->posx + posx - sizeHsq;
                p.posy = itr->posy + 1 + posy - sizeWsq;
                retlist->push_back(p);
                }
                else
                {
                p.posx = itr->posx + 1;
                p.posy = itr->posy - 1;
                worklist.push_back(p);
                p.posx = itr->posx + 1 + posx - sizeHsq;
                p.posy = itr->posy - 1 + posy - sizeWsq;
                retlist->push_back(p);
                p.posx = itr->posx + 1;
                p.posy = itr->posy;
                worklist.push_back(p);
                p.posx = itr->posx + 1 + posx - sizeHsq;
                p.posy = itr->posy + posy - sizeWsq;
                retlist->push_back(p);
                p.posx = itr->posx;
                p.posy = itr->posy - 1;
                worklist.push_back(p);
                p.posx = itr->posx + posx - sizeHsq;
                p.posy = itr->posy - 1 + posy - sizeWsq;
                retlist->push_back(p);
                }

            }
            else
            {
                if(itr->posy > sizeWsq)
                {
                p.posx = itr->posx - 1;
                p.posy = itr->posy + 1;
                worklist.push_back(p);
                p.posx = itr->posx - 1 + posx - sizeHsq;
                p.posy = itr->posy + 1 + posy - sizeWsq;
                retlist->push_back(p);
                p.posx = itr->posx;
                p.posy = itr->posy + 1;
                worklist.push_back(p);
                p.posx = itr->posx + posx - sizeHsq;
                p.posy = itr->posy + 1 + posy - sizeWsq;
                retlist->push_back(p);
                p.posx = itr->posx - 1;
                p.posy = itr->posy;
                worklist.push_back(p);
                p.posx = itr->posx - 1 + posx - sizeHsq;
                p.posy = itr->posy + posy - sizeWsq;
                retlist->push_back(p);
                }
                else
                {
                p.posx = itr->posx - 1;
                p.posy = itr->posy - 1;
                worklist.push_back(p);
                p.posx = itr->posx - 1 + posx - sizeHsq;
                p.posy = itr->posy - 1 + posy - sizeWsq;
                retlist->push_back(p);
                p.posx = itr->posx - 1;
                p.posy = itr->posy;
                worklist.push_back(p);
                p.posx = itr->posx - 1 + posx - sizeHsq;
                p.posy = itr->posy + posy - sizeWsq;
                retlist->push_back(p);
                p.posx = itr->posx;
                p.posy = itr->posy - 1;
                worklist.push_back(p);
                p.posx = itr->posx + posx - sizeHsq;
                p.posy = itr->posy - 1 + posy - sizeWsq;
                retlist->push_back(p);
                }
            }

        worklist.erase(itr++);
    };
    return retlist;
};

void LOSfinder::clearLOS()
{
    worklist.clear();
};