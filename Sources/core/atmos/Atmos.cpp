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
    load_grid_ns_ = 0;
    unload_grid_ns_ = 0;
    movement_processing_ns_ = 0;

    (*texts_)["AtmosGridProcessing"].SetUpdater
    ([&](std::string* str)
    {
        std::stringstream ss;
        ss << "Atmos grid processing: "
           << (grid_processing_ns_ * 1.0) / 1000000.0
           << " ms";
        *str = ss.str();
    }).SetFreq(1000);

    (*texts_)["AtmosGridLoad"].SetUpdater
    ([&](std::string* str)
    {
        std::stringstream ss;
        ss << "Atmos grid load: "
           << (load_grid_ns_ * 1.0) / 1000000.0
           << " ms";
        *str = ss.str();
    }).SetFreq(1000);

    (*texts_)["AtmosGridUnload"].SetUpdater
    ([&](std::string* str)
    {
        std::stringstream ss;
        ss << "Atmos grid unload: "
           << (unload_grid_ns_ * 1.0) / 1000000.0
           << " ms";
        *str = ss.str();
    }).SetFreq(1000);

    (*texts_)["AtmosMove"].SetUpdater
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
    x_shuffle_.resize(x);
    y_shuffle_.resize(y);
    z_size_ = z;

    for (size_t i = 0; i < dir_shuffle_.size(); ++i)
    {
        dir_shuffle_[i] = i;
    }
    for (size_t i = 0; i < x_shuffle_.size(); ++i)
    {
        x_shuffle_[i] = i;
    }
    for (size_t i = 0; i < y_shuffle_.size(); ++i)
    {
        y_shuffle_[i] = i;
    }

    delete grid_;
    grid_ = new AtmosGrid(x, y);
}

void Atmosphere::Process()
{
    QElapsedTimer timer;
    timer.start();
    LoadDataToGrid();
    load_grid_ns_ = (load_grid_ns_ + timer.nsecsElapsed()) / 2;
    timer.start();
    grid_->Process();
    grid_processing_ns_ = (grid_processing_ns_ + timer.nsecsElapsed()) / 2;
    timer.start();
    UnloadDataFromGrid();
    unload_grid_ns_ = (unload_grid_ns_ + timer.nsecsElapsed()) / 2;
}

void Atmosphere::ProcessMove()
{
    QElapsedTimer timer;
    timer.start();
    // TODO: this takes abou 70% of atmos processing time
    // Some wind variable for each tile?
    for (size_t z_counter = 0; z_counter < z_size_; ++z_counter)
    {
       // ShuffleX();
       // ShuffleY();
       // ShuffleDir();
        for (size_t x_sh = 0; x_sh < x_shuffle_.size(); ++x_sh)
        {
            size_t x_counter = x_sh;//x_shuffle_[x_sh];
            for (size_t y_sh = 0; y_sh < y_shuffle_.size(); ++y_sh)
            {
                size_t y_counter = y_sh;//y_shuffle_[y_sh];

                ProcessTileMove(x_counter, y_counter, z_counter);
            }
        }
    }
    movement_processing_ns_
        = (movement_processing_ns_ + timer.nsecsElapsed()) / 2;
}

void Atmosphere::LoadDataToGrid()
{
    auto& squares = map_->GetSquares();
    for (int y = 0; y < map_->GetHeight(); ++y)
    {
        for (int x = 0; x < map_->GetWidth(); ++x)
        {
            auto tile = squares[y][x][0];

            AtmosGrid::Cell& cell = grid_->At(x, y);
            cell.ResetPassable();

            if (!CanPass(tile->GetPassable(D_UP), Passable::AIR))
            {
                cell.SetUnpassable(AtmosGrid::Cell::UP);
            }
            if (!CanPass(tile->GetPassable(D_DOWN), Passable::AIR))
            {
                cell.SetUnpassable(AtmosGrid::Cell::DOWN);
            }
            if (!CanPass(tile->GetPassable(D_LEFT), Passable::AIR))
            {
                cell.SetUnpassable(AtmosGrid::Cell::LEFT);
            }
            if (!CanPass(tile->GetPassable(D_RIGHT), Passable::AIR))
            {
                cell.SetUnpassable(AtmosGrid::Cell::RIGHT);
            }
            if (!CanPass(tile->GetPassable(D_ALL), Passable::AIR))
            {
                cell.SetUnpassable(AtmosGrid::Cell::CENTER);
            }

            if (tile->GetTurf()->GetAtmosState() == SPACE)
            {
                cell.flags |= AtmosGrid::Cell::SPACE;
            }

            AtmosHolder* holder = tile->GetAtmosHolder();

            cell.energy = holder->GetEnergy();

            for (int i = 0; i < GASES_NUM; ++i)
            {
                cell.gases[i] = holder->GetGase(i);
            }
        }
    }
}

void Atmosphere::UnloadDataFromGrid()
{
    auto& squares = map_->GetSquares();
    for (int y = 0; y < map_->GetHeight(); ++y)
    {
        for (int x = 0; x < map_->GetWidth(); ++x)
        {
            auto tile = squares[y][x][0];

            AtmosGrid::Cell& cell = grid_->At(x, y);


            AtmosHolder* holder = tile->GetAtmosHolder();
            holder->Truncate();
            if (cell.flags & AtmosGrid::Cell::SPACE)
            {
                continue;
            }

            for (int i = 0; i < GASES_NUM; ++i)
            {
                holder->AddGase(i, cell.gases[i]);
            }

            holder->AddEnergy(cell.energy);
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

    int max_diff = 0;
    Dir dir;

    for (size_t d_sh = 0; d_sh < dir_shuffle_.size(); ++d_sh)
    {
        Dir local = d_sh;

        auto neighbour = tile->GetNeighbourImpl(local);
        int tile_pressure = tile->GetAtmosHolder()->GetPressure();
        int neighbour_pressure = neighbour->GetAtmosHolder()->GetPressure();
        int local_diff = tile_pressure - neighbour_pressure;
        if (local_diff > max_diff)
        {
            max_diff = local_diff;
            dir = local;
        }
    }

    if (max_diff < PRESSURE_MOVE_BORDER)
    {
        return;
    }

    IdPtr<CubeTile> neighbour = tile->GetNeighbourImpl(dir);

    if (!CanPass(tile->GetPassable(dir), Passable::AIR))
    {
        tile->BumpByGas(dir, true);
        return;
    }

    if (   !CanPass(neighbour->GetPassable(D_ALL), Passable::AIR)
        || !CanPass(neighbour->GetPassable(helpers::revert_dir(dir)), Passable::AIR))
    {
        neighbour->BumpByGas(dir);
        return;
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

void Atmosphere::ShuffleX()
{
    for (size_t i = 0; i < x_shuffle_.size(); ++i)
    {
        x_shuffle_[i] = i;
    }
    std::random_shuffle(
        x_shuffle_.begin(),
        x_shuffle_.end(),
        [&](int v) { return random_->RandomShuffle(v); });
}

void Atmosphere::ShuffleY()
{
    for (size_t i = 0; i < y_shuffle_.size(); ++i)
    {
        y_shuffle_[i] = i;
    }
    std::random_shuffle(
        y_shuffle_.begin(),
        y_shuffle_.end(),
        [&](int v) { return random_->RandomShuffle(v); });
}

void Atmosphere::ShuffleDir()
{
    for (size_t i = 0; i < dir_shuffle_.size(); ++i)
    {
        dir_shuffle_[i] = i;
    }
    std::random_shuffle(
        dir_shuffle_.begin(),
        dir_shuffle_.end(),
        [&](int v) { return random_->RandomShuffle(v); });
}
