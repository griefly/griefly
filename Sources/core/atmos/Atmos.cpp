#include "Atmos.h"

#include <QElapsedTimer>

#include <algorithm>

#include "../SyncRandom.h"
#include "../Helpers.h"

#include "AtmosGrid.h"

#include "representation/Text.h"

Atmosphere::Atmosphere(SyncRandom* random, IMapMaster* map, TextPainter *texts)
    : random_(random),
      map_(map),
      texts_(texts)
{
    grid_processing_ns_ = 0;
    unload_grid_ns_ = 0;
    movement_processing_ns_ = 0;

    x_size_ = 0;
    y_size_ = 0;
    z_size_ = 0;

    (*texts_)["{Perf}AtmosGridProcessing"].SetUpdater
    ([&](std::string* str)
    {
        std::stringstream ss;
        ss << "Atmos grid processing: "
           << (grid_processing_ns_ * 1.0) / 1000000.0
           << " ms";
        *str = ss.str();
    }).SetFreq(1000);

    (*texts_)["{Perf}AtmosGridUnload"].SetUpdater
    ([&](std::string* str)
    {
        std::stringstream ss;
        ss << "Atmos grid unload: "
           << (unload_grid_ns_ * 1.0) / 1000000.0
           << " ms";
        *str = ss.str();
    }).SetFreq(1000);

    (*texts_)["{Perf}AtmosMove"].SetUpdater
    ([&](std::string* str)
    {
        std::stringstream ss;
        ss << "Atmos move processing: "
           << (movement_processing_ns_ * 1.0) / 1000000.0
           << " ms";
        *str = ss.str();
    }).SetFreq(1000);

    qDebug() << "Atmosphere load";
    z_size_ = 1;
    grid_ = nullptr;
}

void Atmosphere::Resize(size_t x, size_t y, size_t z)
{
    x_size_ = x;
    y_size_ = y;
    z_size_ = z;

    delete grid_;
    grid_ = new AtmosGrid(x_size_, y_size_);
}

void Atmosphere::Process()
{
    QElapsedTimer timer;
    timer.start();
    grid_->Process();
    grid_processing_ns_ = (grid_processing_ns_ + timer.nsecsElapsed()) / 2;
    timer.start();
    UnloadDataFromGrid();
    unload_grid_ns_ = (unload_grid_ns_ + timer.nsecsElapsed()) / 2;
}

void Atmosphere::UnloadDataFromGrid()
{
    for (int z = 0; z < z_size_; ++z)
    {
        for (int x = 0; x < x_size_; ++x)
        {
            for (int y = 0; y < y_size_; ++y)
            {
                AtmosGrid::Cell& cell = grid_->At(x, y);
                UpdateMacroParams(&cell.data);
            }
        }
    }
}

const unsigned int PRESSURE_MOVE_BORDER = 1000;

void Atmosphere::ProcessTileMove(size_t x, size_t y, size_t z)
{   
    auto tile = map_->GetSquares()[x][y][z];

    if (tile->GetTurf()->GetAtmosState() == NON_SIMULATED)
    {
        return;
    }
    if (tile->GetTurf()->GetAtmosState() == SPACE)
    {
        return;
    }

    for (Dir dir = 0; dir < 4; ++dir)
    {
        auto neighbour = tile->GetNeighbourImpl(dir);

        if (!(   (   neighbour->GetTurf()->GetAtmosState() == NON_SIMULATED
                  && PRESSURE_MOVE_BORDER < tile->GetAtmosHolder()->GetPressure())
              || (neighbour->GetAtmosHolder()->GetPressure() + PRESSURE_MOVE_BORDER
                  < tile->GetAtmosHolder()->GetPressure())))
        {
            continue;
        }

        if (!CanPass(tile->GetPassable(dir), Passable::AIR))
        {
            tile->BumpByGas(dir, true);
            continue;
        }

        if (   !CanPass(neighbour->GetPassable(D_ALL), Passable::AIR)
            || !CanPass(neighbour->GetPassable(helpers::revert_dir(dir)), Passable::AIR))
        {
            neighbour->BumpByGas(dir);
            continue;
        }

        if (tile->GetInsideList().size())
        {
            auto i = tile->GetInsideList().rbegin();
            while (   (i != tile->GetInsideList().rend())
                   && ((*i)->passable_level == Passable::EMPTY))
            {
                ++i;
            }
            if (i != tile->GetInsideList().rend())
            {
                (*i)->ApplyForce(DirToVDir[dir]);
            }
        }
    }
}

void Atmosphere::ProcessMove()
{
    QElapsedTimer timer;
    timer.start();
    // TODO: this takes abou 70% of atmos processing time
    // Some wind variable for each tile?
    for (int z = 0; z < z_size_; ++z)
    {
        for (int x = 0; x < x_size_; ++x)
        {
            for (int y = 0; y < y_size_; ++y)
            {
                ProcessTileMove(x, y, z);
            }
        }
    }
    movement_processing_ns_
            = (movement_processing_ns_ + timer.nsecsElapsed()) / 2;
}

void Atmosphere::SetFlags(size_t x, size_t y, size_t z, IAtmosphere::Flags flags)
{
    // TODO: fix grid x and y
    grid_->At(y, x).flags = flags;
}

void Atmosphere::LoadGrid()
{
    auto& squares = map_->GetSquares();
    for (int z = 0; z < z_size_; ++z)
    {
        for (int x = 0; x < x_size_; ++x)
        {
            for (int y = 0; y < y_size_; ++y)
            {
                auto& tile = squares[x][y][z];
                tile->UpdateAtmosPassable();
                AtmosHolder* holder = tile->GetAtmosHolder();
                AtmosGrid::Cell& cell = grid_->At(y, x);

                cell.data = holder->data_;
                holder->SetAtmosData(&cell.data);
            }
        }
    }
}
