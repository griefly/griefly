#include <math.h>
#include <sstream>
#include <iostream>
#include <assert.h>

#include <QDebug>

#include "Map.h"
#include "objects/MainObject.h"
#include "objects/Tile.h"
#include "Game.h"
#include "ObjectFactory.h"
#include "VisiblePoints.h"
#include "Helpers.h"
#include "Helpers.h"
#include "objects/Creator.h"
#include "AutogenMetadata.h"
#include "representation/Representation.h"

void MapMaster::FillAtmosphere()
{
    for (int z = 0; z < GetMapD(); ++z)
        for (int x = 0; x < GetMapW(); ++x)
            for (int y = 0; y < GetMapH(); ++y)
                if (   squares[x][y][z]->GetTurf()
                    && squares[x][y][z]->GetTurf()->GetAtmosState() != SPACE
                    && squares[x][y][z]->GetTurf()->GetAtmosState() != NON_SIMULATED
                    && CanPass(squares[x][y][z]->GetPassable(D_ALL), Passable::AIR))
                {
                    auto a = squares[x][y][z]->GetAtmosHolder();
                    a->AddGase(NYTROGEN, 750);
                    a->AddGase(OXYGEN, 230);
                    a->AddGase(CO2, 1);
                    a->AddEnergy(1000);
                }
}

void MapMaster::SaveToMapGen(const std::string& name)
{
    std::fstream sfile;
    sfile.open(name, std::ios_base::out | std::ios_base::trunc);
    if(sfile.fail()) 
    {
        SYSTEM_STREAM << "Error open " << name << std::endl; 
        return;
    }

    sfile << GetMapW() << std::endl;
    sfile << GetMapH() << std::endl;
    sfile << GetMapD() << std::endl;
//    sfile << GetCreator() << std::endl;

    std::map<std::string, std::string> dummy;

    for (int z = 0; z < GetMapD(); ++z)
        for (int x = 0; x < GetMapW(); ++x)
            for (int y = 0; y < GetMapH(); ++y)
            {
                if (auto t = squares[x][y][z]->GetTurf())
                {
                    sfile << t->T_ITEM() << " ";
                    sfile << x << " ";
                    sfile << y << " ";
                    sfile << z << " ";
                    sfile << std::endl;
                }
                auto& il = squares[x][y][z]->GetInsideList();
                for (auto it = il.begin(); it != il.end(); ++it)
                {
                    sfile << (*it)->T_ITEM() << " ";
                    sfile << x << " ";
                    sfile << y << " ";
                    sfile << z << " ";
                    sfile << std::endl;
                }
                std::stringstream ss;
                WrapWriteMessage(ss, dummy);
                sfile << ss.str();
            }
}

void MapMaster::LoadFromMapGen(const std::string& name)
{
    GetFactory().ClearMap();

    std::fstream sfile;
    sfile.open(name, std::ios_base::in);
    if(sfile.fail()) 
    {
        SYSTEM_STREAM << "Error open " << name << std::endl; 
        return;
    }

    std::stringstream ss;

    sfile.seekg (0, std::ios::end);
    std::streamoff length = sfile.tellg();
    sfile.seekg (0, std::ios::beg);
    char* buff = new char[static_cast<size_t>(length)];

    sfile.read(buff, length);
    sfile.close();
    ss.write(buff, length);
    delete[] buff;

    GetFactory().BeginWorldCreation();

    int x, y, z;
    ss >> x;
    ss >> y;
    ss >> z;

 //   size_t creator;
//    sfile >> creator;

    MakeTiles(x, y, z);

   // qDebug() << "Begin loading cycle";
    while (ss)
    {
        std::string t_item;
        size_t x, y, z;
        ss >> t_item;
        if (!ss)
        {
            continue;
        }
        ss >> x;
        if (!ss)
        {
            continue;
        }
        ss >> y;
        if (!ss)
        {
            continue;
        }
        ss >> z;
        if (!ss)
        {
            continue;
        }

        auto i = GetFactory().Create<IOnMapObject>(t_item);

        std::map<std::string, std::string> variables;
        WrapReadMessage(ss, variables);

        for (auto it = variables.begin(); it != variables.end(); ++it)
        {
            if ((it->second.size() == 0) || (it->first.size() == 0))
            {
                continue;
            }
            std::stringstream local_variable;
            local_variable << it->second;

            //qDebug() << it->second.c_str();

            get_setters_for_types()[t_item][it->first](i.operator*(), local_variable);
        }

        if (id_ptr_on<ITurf> t = i)
        {
            if (squares[x][y][z]->GetTurf())
            {
                SYSTEM_STREAM << "DOUBLE TURF!" << std::endl;
            }
            squares[x][y][z]->SetTurf(t);
        }
        else
        {
            squares[x][y][z]->AddItem(i);
        }
    }
   GetFactory().FinishWorldCreation();
}

void MapMaster::Represent()
{
    if(!GetGame().GetVisiblePoints())
    {
        return;
    }

    for(int i = 0; i < MAX_LEVEL; ++i)
    {
        auto it2 = GetGame().GetVisiblePoints()->begin();
        while(it2 != GetGame().GetVisiblePoints()->end())
        {
            auto sq = squares[it2->posx][it2->posy][it2->posz];
            auto& in_list = sq->GetInsideList();

            for (auto list_it = in_list.begin(); list_it != in_list.end(); ++list_it)
            {
                if ((*list_it)->v_level == i)
                {
                    (*list_it)->Represent();
                }
            }

            auto trf = squares[it2->posx][it2->posy][it2->posz]->GetTurf();
            if (trf.valid() && trf->v_level == i)
            {
                trf->Represent();
            }
            ++it2;
        }
    }
    auto it2 = GetGame().GetVisiblePoints()->begin();
    while(it2 != GetGame().GetVisiblePoints()->end())
    {
        auto sq = squares[it2->posx][it2->posy][it2->posz];
        auto& in_list = sq->GetInsideList();

        for (auto list_it = in_list.begin(); list_it != in_list.end(); ++list_it)
        {
            if ((*list_it)->v_level >= MAX_LEVEL)
            {
                (*list_it)->Represent();
            }
        }

        auto trf = squares[it2->posx][it2->posy][it2->posz]->GetTurf();
        if (trf.valid() && trf->v_level >= MAX_LEVEL)
        {
            trf->Represent();
        }
        ++it2;
    }
}

void MapMaster::GenerateFrame()
{
    if(!GetGame().GetVisiblePoints())
    {
        return;
    }

    Represent();
    GetMob()->GenerateInterfaceForFrame();


    // TODO: reset all shifts
    GetRepresentation().SetCameraForFrame(GetMob()->GetX(), GetMob()->GetY());
    GetRepresentation().Swap();
}

void MapMaster::MakeTiles(int new_map_x, int new_map_y, int new_map_z)
{
    ResizeMap(new_map_x, new_map_y, new_map_z);
    for(int x = 0; x < GetMapW(); x++)
    {
        for(int y = 0; y < GetMapH(); y++) 
        {
            for (int z = 0; z < GetMapD(); z++)
            {
                auto loc = GetFactory().Create<CubeTile>(CubeTile::T_ITEM_S());
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
    atmosphere.Resize(new_map_x, new_map_y);
}

MapMaster::MapMaster()
{

}

PassableLevel MapMaster::GetPassable(int posx, int posy, int posz, Dir direct)
{
    return squares[posx][posy][posz]->GetPassable(direct);
}

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
}

bool MapMaster::IsTransparent(int posx, int posy, int posz)
{
    if (!helpers::check_borders(&posx, &posy, &posz))
    {
        return false;
    }
    return squares[posx][posy][posz]->IsTransparent();
}

MapMaster* map_master_ = 0;
MapMaster& GetMap()
{
    return *map_master_;
}

void SetMapMaster(MapMaster* map_master)
{
    map_master_ = map_master;
}

bool IsMapValid()
{
    return map_master_ != nullptr;
}

const int rayMultiplier = 2;

int pos2corner(int pos) {
    return pos*rayMultiplier;
}

int corner2pos(int corner) {
    return corner/rayMultiplier;
}

int sign(int value) {
    if (value > 0) {
        return 1;
    } else if (value < 0) {
        return -1;
    }

    return 0;
}

bool checkCorner(point p) {
    int x = corner2pos(p.posx);
    int y = corner2pos(p.posy);
    int z = corner2pos(p.posz);
    return helpers::check_borders(&x, &y, &z);
}


point cornerPoint2point(point p) {
    point retval = {corner2pos(p.posx), corner2pos(p.posy), corner2pos(p.posz)};
    return retval;
}

bool isTransparent(point p) {
    point tilePoint = cornerPoint2point(p);
    return GetMap().IsTransparent(tilePoint.posx, tilePoint.posy, tilePoint.posz);
}

bool bresenX(point source, point target) {
    int y = source.posy;
    int error = 0;
    int deltaX = abs(source.posx - target.posx);
    int deltaerr = abs(source.posy - target.posy);
    int deltastep = sign(target.posy - source.posy);
    int incrstep = sign(target.posx - source.posx);
    for (int x = source.posx; x != target.posx; x += incrstep) {
        if ((x % rayMultiplier) == 0 && (y % rayMultiplier) == 0) {
            // when in corner check side neighbours
            // if both of them are not transparent then corner is not transparent
            point left_neighbour = {x+incrstep, y, source.posz};
            point right_neighbour = {x, y+deltastep, source.posz};
            if (!isTransparent(left_neighbour) && !isTransparent(right_neighbour)) {
                return false;
            }
        } else if (x % rayMultiplier == 0) {
            // when ray hits an edge check both tiles - current and previous. Since ray travels through edge both of them 
            // must be transparent
            point left_neighbour = {x-incrstep, y, source.posz};
            point right_neighbour = {x+incrstep, y, source.posz};
            if (!isTransparent(left_neighbour) || !isTransparent(right_neighbour)) {
                return false;
            }
        } else if (y % rayMultiplier == 0) {
            // second case of edge handling
            point left_neighbour = {x, y-deltastep, source.posz};
            point right_neighbour = {x, y+deltastep, source.posz};
            if (!isTransparent(left_neighbour) || !isTransparent(right_neighbour)) {
                return false;
            }
        } else {
            point new_point = {x, y, source.posz};
            if (!isTransparent(new_point)) {
                return false;
            }
        }

        error += deltaerr;
        if (error >= deltaX) {
            y += deltastep;
            error -= deltaX;
        }
    }

    return true;
}

bool bresenY(point source, point target) {
    int x = source.posx;
    int error = 0;
    int deltaY = abs(source.posy - target.posy);
    int deltaerr = abs(source.posx - target.posx);
    int deltastep = sign(target.posx - source.posx);
    int incrstep = sign(target.posy - source.posy);
    for (int y = source.posy; y != target.posy; y += incrstep) {
        if ((x % rayMultiplier) == 0 && (y % rayMultiplier) == 0) {
            // when in corner check side neighbours
            // if both of them are not transparent then corner is not transparent
            point left_neighbour = {x+deltastep, y, source.posz};
            point right_neighbour = {x, y+incrstep, source.posz};
            if (!isTransparent(left_neighbour) && !isTransparent(right_neighbour)) {
                return false;
            }
        } else if (x % rayMultiplier == 0) {
            // when ray hits an edge check both tiles. Since ray travels through edge both of them 
            // must be transparent
            point left_neighbour = {x-deltastep, y, source.posz};
            point right_neighbour = {x+deltastep, y, source.posz};
            if (!isTransparent(left_neighbour) || !isTransparent(right_neighbour)) {
                return false;
            }
        } else if (y % rayMultiplier == 0) {
            // second case of edge handling
            point left_neighbour = {x, y-incrstep, source.posz};
            point right_neighbour = {x, y+incrstep, source.posz};
            if (!isTransparent(left_neighbour) || !isTransparent(right_neighbour)) {
                return false;
            }
        } else {
            point new_point = {x, y, source.posz};

            if (!isTransparent(new_point)) {
                return false;
            }
        }

        error += deltaerr;
        if (error >= deltaY) {
            x += deltastep;
            error -= deltaY;
        }
    }

    return true;
}

bool rayTrace(point source, point target) {
    // run Bresenham's line algorithm
    if (abs(source.posx - target.posx) > abs(source.posy - target.posy)) {
        return bresenX(source, target);
    } else {
        return bresenY(source, target);
    }

    return false;
}

void markTilesOfCornerAsVisible(std::list<point>* retlist, point at, point center, char visibility[]) {
    for (int dx = -1; dx <= 0; dx++) {
        for (int dy = -1; dy <= 0; dy++) {
            point p = {at.posx+dx, at.posy+dy, at.posz};
            if (!helpers::check_borders(&p.posx, &p.posy, &p.posz)) {
                continue;
            }

            int visX = (p.posx - center.posx + sizeWsq);
            int visY = (p.posy - center.posy + sizeHsq);
            int visIdx = 2*sizeHsq*visX + visY;

            if (visIdx < 0) {
                continue;
            }

            if (visibility[visIdx] == 1) {
                continue;
            }

            retlist->push_back(p);
            visibility[visIdx] = 1;
        }
    }
}

// LOSfinder::calculateVisisble calculates visibility list of map from given map point
// The line-of-sigh check works as follows:
// from the center of given tile to every tile corner of the viewport ray is casted
// ray is casted from center of tile to corners, so it is never aligned to either axis
// if ray passes only transparent tiles on its way to given corner then the corner is visible
// if ray passes through corner then it checks sibling tiles. If both of them are not transparent then ray blocks
// if ray passes through edge it checks both adjasent tiles. They both must be transparent, otherwise ray blocks
// if tile has at least one visible corner then this tile is visible
// otherwise tile is invisible
std::list<point>* LOSfinder::calculateVisisble(std::list<point>* retlist, int posx, int posy, int posz)
{
    clearLOS();

    auto visibleTiles = new char[4*(sizeHsq+2)*(sizeWsq+2)];
    for (int i = 0; i < 4*(sizeHsq+2)*(sizeWsq+2); i++) {
        visibleTiles[i] = 0;
    }

    point source = {pos2corner(posx)+rayMultiplier/2, pos2corner(posy)+rayMultiplier/2, pos2corner(posz)+1};
    for (int i = -sizeWsq; i < sizeWsq; i++) {
        for (int j = -sizeHsq; j < sizeHsq; j++) {
            point p = {pos2corner(posx+i), pos2corner(posy+j), pos2corner(posz)};
            if (!checkCorner(p)) {
                continue;
            }

            // TODO: we can check that all siblings of this corner are visible
            // so check is unnessesary

            if (rayTrace(source, p)) {
                // add all tiles with this corner to visible list
                markTilesOfCornerAsVisible(retlist, cornerPoint2point(p), cornerPoint2point(source), visibleTiles);
            }
        }
    }

    delete[] visibleTiles;
    return retlist;
}

void LOSfinder::clearLOS()
{
    worklist.clear();
}
