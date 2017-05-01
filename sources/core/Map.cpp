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
