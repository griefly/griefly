#include <cmath>
#include <iostream>
#include <cassert>

#include <QDebug>

#include "Map.h"
#include "objects/MainObject.h"
#include "objects/Tile.h"
#include "Game.h"
#include "ObjectFactory.h"
#include "Helpers.h"
#include "AutogenMetadata.h"
#include "representation/Representation.h"

void Map::FillTilesAtmosHolders()
{
    for (int z = 0; z < GetDepth(); ++z)
    {
        for (int x = 0; x < GetWidth(); ++x)
        {
            for (int y = 0; y < GetHeight(); ++y)
            {
                if (   squares_[x][y][z]->GetTurf()
                    && squares_[x][y][z]->GetTurf()->GetAtmosState() != atmos::SPACE
                    && squares_[x][y][z]->GetTurf()->GetAtmosState() != atmos::NON_SIMULATED)
                {
                    // It is not passable then still fill with air cause of doors
                    auto holder = squares_[x][y][z]->GetAtmosHolder();
                    atmos::AddDefaultValues(holder);
                }
            }
        }
    }
}

void Map::Represent()
{
    if (!GetVisiblePoints())
    {
        return;
    }

    for (int i = 0; i < MAX_LEVEL; ++i)
    {
        auto it2 = GetVisiblePoints()->begin();
        while (it2 != GetVisiblePoints()->end())
        {
            auto sq = squares_[it2->posx][it2->posy][it2->posz];
            auto& in_list = sq->GetInsideList();

            for (auto list_it = in_list.begin(); list_it != in_list.end(); ++list_it)
            {
                if ((*list_it)->v_level == i)
                {
                    (*list_it)->Represent();
                }
            }

            auto trf = squares_[it2->posx][it2->posy][it2->posz]->GetTurf();
            if (trf.IsValid() && trf->v_level == i)
            {
                trf->Represent();
            }
            ++it2;
        }
    }
    auto it2 = GetVisiblePoints()->begin();
    while (it2 != GetVisiblePoints()->end())
    {
        auto sq = squares_[it2->posx][it2->posy][it2->posz];
        auto& in_list = sq->GetInsideList();

        for (auto list_it = in_list.begin(); list_it != in_list.end(); ++list_it)
        {
            if ((*list_it)->v_level >= MAX_LEVEL)
            {
                (*list_it)->Represent();
            }
        }

        auto trf = squares_[it2->posx][it2->posy][it2->posz]->GetTurf();
        if (trf.IsValid() && trf->v_level >= MAX_LEVEL)
        {
            trf->Represent();
        }
        ++it2;
    }
}

void Map::Resize(int new_x, int new_y, int new_z)
{
    if (   new_x < 1
        || new_y < 1
        || new_z < 1)
    {
        KvAbort(QString("Incorrect map new size (%1, %2, %3)").arg(new_x).arg(new_y).arg(new_z));
    }

    squares_.resize(new_x);
    for (int x = 0; x < new_x; ++x)
    {
        squares_[x].resize(new_y);
        for (int y = 0; y < new_y; ++y)
        {
            squares_[x][y].resize(new_z);
        }
    }
}

Map::Map()
    : los_calculator_(this)
{
    // Nothing
}

Map::~Map()
{
    // Nothing
}

Map::SqType& Map::At(int x, int y, int z)
{
    return squares_[x][y][z];
}

const Map::SqType& Map::At(int x, int y, int z) const
{
    return squares_[x][y][z];
}

int Map::GetWidth() const
{
    return squares_.size();
}

int Map::GetHeight() const
{
    if (!GetWidth())
    {
        return 0;
    }
    return squares_[0].size();
}

int Map::GetDepth() const
{
    if (!GetHeight())
    {
        return 0;
    }
    return squares_[0][0].size();
}

bool Map::IsTransparent(int posx, int posy, int posz)
{
    return At(posx, posy, posz)->IsTransparent();
}

bool Map::IsTileVisible(quint32 tile_id)
{
    auto l = GetVisiblePoints();
    if (!l)
    {
        return false;
    }
    for (auto it = l->begin(); it != l->end(); ++it)
    {
        if (tile_id == squares_[it->posx][it->posy][it->posz].Id())
        {
            return true;
        }
    }
    return false;
}

std::list<PosPoint>* Map::GetVisiblePoints()
{
    return &visible_points_;
}

void Map::CalculateLos(std::list<PosPoint> *retval, int posx, int posy, int posz)
{
    los_calculator_.Calculate(retval, posx, posy, posz);
}

const int RAY_MULTIPLIER = 2;

int LosCalculator::PosToCorner(int pos)
{
    return pos * RAY_MULTIPLIER;
}

int LosCalculator::CornerToPos(int corner)
{
    return corner / RAY_MULTIPLIER;
}

int LosCalculator::Sign(int value)
{
    if (value > 0)
    {
        return 1;
    }
    else if (value < 0)
    {
        return -1;
    }
    return 0;
}

bool LosCalculator::CheckCorner(PosPoint p)
{
    int x = CornerToPos(p.posx);
    int y = CornerToPos(p.posy);
    int z = CornerToPos(p.posz);
    return CheckBorders(x, y, z);
}

bool LosCalculator::CheckBorders(const int x, const int y, const int z)
{
    if (   x < 0
        || x >= map_->GetWidth())
    {
        return false;
    }
    if (   y < 0
        || y >= map_->GetHeight())
    {
        return false;
    }
    if (   z < 0
        || z >= map_->GetDepth())
    {
        return false;
    }
    return true;
}


PosPoint LosCalculator::CornerPointToPoint(PosPoint p)
{
    PosPoint retval(CornerToPos(p.posx), CornerToPos(p.posy), CornerToPos(p.posz));
    return retval;
}

bool LosCalculator::IsTransparent(PosPoint p)
{
    PosPoint tilePoint = CornerPointToPoint(p);
    return map_->IsTransparent(tilePoint.posx, tilePoint.posy, tilePoint.posz);
}

bool LosCalculator::BresenX(PosPoint source, PosPoint target)
{
    int y = source.posy;
    int error = 0;
    int delta_x = std::abs(source.posx - target.posx);
    int deltaerr = std::abs(source.posy - target.posy);
    int deltastep = Sign(target.posy - source.posy);
    int incrstep = Sign(target.posx - source.posx);
    for (int x = source.posx; x != target.posx; x += incrstep)
    {
        if ((x % RAY_MULTIPLIER) == 0 && (y % RAY_MULTIPLIER) == 0)
        {
            // when in corner check side neighbours
            // if both of them are not transparent then corner is not transparent
            PosPoint left_neighbour(x + incrstep, y, source.posz);
            PosPoint right_neighbour(x, y + deltastep, source.posz);
            if (!IsTransparent(left_neighbour) && !IsTransparent(right_neighbour))
            {
                return false;
            }
        }
        else if (x % RAY_MULTIPLIER == 0)
        {
            // when ray hits an edge check both tiles - current and previous. Since ray travels through edge both of them 
            // must be transparent
            PosPoint left_neighbour(x - incrstep, y, source.posz);
            PosPoint right_neighbour(x + incrstep, y, source.posz);
            if (!IsTransparent(left_neighbour) || !IsTransparent(right_neighbour))
            {
                return false;
            }
        }
        else if (y % RAY_MULTIPLIER == 0)
        {
            // second case of edge handling
            PosPoint left_neighbour(x, y - deltastep, source.posz);
            PosPoint right_neighbour(x, y + deltastep, source.posz);
            if (!IsTransparent(left_neighbour) || !IsTransparent(right_neighbour))
            {
                return false;
            }
        }
        else
        {
            PosPoint new_point(x, y, source.posz);
            if (!IsTransparent(new_point))
            {
                return false;
            }
        }

        error += deltaerr;
        if (error >= delta_x)
        {
            y += deltastep;
            error -= delta_x;
        }
    }

    return true;
}

bool LosCalculator::BresenY(PosPoint source, PosPoint target)
{
    int x = source.posx;
    int error = 0;
    int delta_y = std::abs(source.posy - target.posy);
    int deltaerr = std::abs(source.posx - target.posx);
    int deltastep = Sign(target.posx - source.posx);
    int incrstep = Sign(target.posy - source.posy);
    for (int y = source.posy; y != target.posy; y += incrstep)
    {
        if ((x % RAY_MULTIPLIER) == 0 && (y % RAY_MULTIPLIER) == 0)
        {
            // when in corner check side neighbours
            // if both of them are not transparent then corner is not transparent
            PosPoint left_neighbour(x + deltastep, y, source.posz);
            PosPoint right_neighbour(x, y + incrstep, source.posz);
            if (!IsTransparent(left_neighbour) && !IsTransparent(right_neighbour))
            {
                return false;
            }
        }
        else if (x % RAY_MULTIPLIER == 0)
        {
            // when ray hits an edge check both tiles. Since ray travels through edge both of them 
            // must be transparent
            PosPoint left_neighbour(x - deltastep, y, source.posz);
            PosPoint right_neighbour(x + deltastep, y, source.posz);
            if (!IsTransparent(left_neighbour) || !IsTransparent(right_neighbour))
            {
                return false;
            }
        }
        else if (y % RAY_MULTIPLIER == 0)
        {
            // second case of edge handling
            PosPoint left_neighbour(x, y - incrstep, source.posz);
            PosPoint right_neighbour(x, y + incrstep, source.posz);
            if (!IsTransparent(left_neighbour) || !IsTransparent(right_neighbour))
            {
                return false;
            }
        }
        else
        {
            PosPoint new_point(x, y, source.posz);

            if (!IsTransparent(new_point))
            {
                return false;
            }
        }

        error += deltaerr;
        if (error >= delta_y)
        {
            x += deltastep;
            error -= delta_y;
        }
    }

    return true;
}

bool LosCalculator::RayTrace(PosPoint source, PosPoint target)
{
    // run Bresenham's line algorithm
    if (std::abs(source.posx - target.posx) > std::abs(source.posy - target.posy))
    {
        return BresenX(source, target);
    }
    else
    {
        return BresenY(source, target);
    }

    return false;
}

void LosCalculator::MarkTilesOfCornerAsVisible(
        std::list<PosPoint>* retlist,
        PosPoint at,
        PosPoint center,
        std::vector<char>* visibility)
{
    for (int dx = -1; dx <= 0; dx++)
    {
        for (int dy = -1; dy <= 0; dy++)
        {
            PosPoint p(at.posx + dx, at.posy + dy, at.posz);
            if (!CheckBorders(p.posx, p.posy, p.posz))
            {
                continue;
            }

            int vis_x = (p.posx - center.posx + SIZE_W_SQ);
            int vis_y = (p.posy - center.posy + SIZE_H_SQ);
            int vis_idx = 2 * SIZE_H_SQ * vis_x + vis_y;

            if (vis_idx < 0)
            {
                continue;
            }

            if ((*visibility)[vis_idx] == 1)
            {
                continue;
            }

            retlist->push_back(p);
            (*visibility)[vis_idx] = 1;
        }
    }
}

LosCalculator::LosCalculator(IMap* map)
{
    map_ = map;
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
void LosCalculator::Calculate(std::list<PosPoint>* retlist, int posx, int posy, int posz)
{
    const int VISIBLE_TILES_SIZE = 4 * (SIZE_H_SQ + 2) * (SIZE_W_SQ + 2);

    std::vector<char> visible_tiles;
    visible_tiles.resize(VISIBLE_TILES_SIZE);

    for (int i = 0; i < VISIBLE_TILES_SIZE; ++i)
    {
        visible_tiles[i] = 0;
    }

    PosPoint source(
          PosToCorner(posx) + RAY_MULTIPLIER / 2,
          PosToCorner(posy) + RAY_MULTIPLIER / 2,
          PosToCorner(posz) + 1);
    for (int i = -SIZE_W_SQ; i < SIZE_W_SQ; ++i)
    {
        for (int j = -SIZE_H_SQ; j < SIZE_H_SQ; ++j)
        {
            PosPoint p(PosToCorner(posx + i), PosToCorner(posy + j), PosToCorner(posz));
            if (!CheckCorner(p))
            {
                continue;
            }

            // TODO: we can check that all siblings of this corner are visible
            // so check is unnessesary

            if (RayTrace(source, p))
            {
                // add all tiles with this corner to visible list
                MarkTilesOfCornerAsVisible(
                    retlist, CornerPointToPoint(p), CornerPointToPoint(source), &visible_tiles);
            }
        }
    }
}
