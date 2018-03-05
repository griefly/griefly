#include "Atmos.h"

#include <QElapsedTimer>

#include <algorithm>

#include "SynchronizedRandom.h"
#include "objects/Tile.h"

#include "AtmosGrid.h"
#include "objects/PhysicsEngine.h"

using namespace kv;

Atmosphere::Atmosphere()
    : map_(nullptr)
{
    grid_processing_ns_ = 0;
    movement_processing_ns_ = 0;

    x_size_ = 0;
    y_size_ = 0;
    z_size_ = 0;

    qDebug() << "Atmosphere load";
    z_size_ = 1;
    grid_ = nullptr;
}

void Atmosphere::Resize(quint32 x, quint32 y, quint32 z)
{
    x_size_ = x;
    y_size_ = y;
    z_size_ = z;

    delete grid_;
    grid_ = new atmos::AtmosGrid(x_size_, y_size_);
}

void Atmosphere::Process(qint32 game_tick)
{
    AssertGrid();

    QElapsedTimer timer;
    timer.start();
    grid_->Process(game_tick);
    grid_processing_ns_ = (grid_processing_ns_ + timer.nsecsElapsed()) / 2;
    timer.start();
}

const int PRESSURE_MOVE_BORDER = 1000;
const int FLOW_MOVE_BORDER = -15;

void Atmosphere::ProcessTileMove(int x, int y, int z, qint32 game_tick)
{
    atmos::AtmosGrid::Cell& cell = grid_->At(x, y);

    Vector force;

    if (cell.flags & atmos::NO_OBJECTS)
    {
        for (int dir = 0; dir < atmos::DIRS_SIZE; ++dir)
        {
            cell.flows[dir] = 0;
        }
    }
    else
    {
        for (int dir = 0; dir < atmos::DIRS_SIZE; ++dir)
        {
            int flow = cell.flows[dir];
            cell.flows[dir] = 0;
            if (flow <= FLOW_MOVE_BORDER)
            {
                Vector local = DirToVDir(atmos::INDEXES_TO_DIRS[static_cast<int>(dir)]);
                force.x += local.x;
                force.y += local.y;
            }
        }
        if (IsNonZero(force))
        {
            force *= FORCE_UNIT;
            auto tile = map_->At(x, y, z);
            if (tile->GetInsideList().size())
            {
                auto i = tile->GetInsideList().rbegin();
                while (   (i != tile->GetInsideList().rend())
                       && ((*i)->passable_level == passable::EMPTY))
                {
                    ++i;
                }
                if (i != tile->GetInsideList().rend())
                {
                    (*i)->ApplyForce(force);
                }
            }
        }
    }

    // TODO: borders
    if (x == 0 || x >= x_size_ - 1)
    {
        return;
    }
    if (y == 0 || y >= y_size_ - 1)
    {
        return;
    }

    if (!cell.IsPassable(atmos::CENTER_BLOCK))
    {
        return;
    }

    const int UNPASSABLE_FREQUENCY = 6;
    if (((x + y) % UNPASSABLE_FREQUENCY) != (game_tick % UNPASSABLE_FREQUENCY))
    {
        return;
    }

    auto tile = map_->At(x, y, z);
    for (int dir = 0; dir < atmos::DIRS_SIZE; ++dir)
    {
        atmos::AtmosGrid::Cell& nearby = grid_->Get(x, y, atmos::INDEXES_TO_DIRS[dir]);
        if (  (nearby.data.pressure + PRESSURE_MOVE_BORDER)
            < cell.data.pressure)
        {
            if (!cell.IsPassable(atmos::DIRS[dir]))
            {
                Dir bump_dir = atmos::INDEXES_TO_DIRS[dir];
                tile->BumpByGas(bump_dir, true);
                continue;
            }
        }
        else if (  (cell.data.pressure + PRESSURE_MOVE_BORDER)
                 < nearby.data.pressure)
        {
            if (nearby.IsPassable(atmos::REVERT_DIRS[dir]))
            {
                if (!cell.IsPassable(atmos::DIRS[dir]))
                {
                    int revert_dir = atmos::REVERT_DIRS_INDEXES[dir];
                    Dir bump_dir = atmos::INDEXES_TO_DIRS[revert_dir];
                    tile->BumpByGas(bump_dir, true);
                    continue;
                }
            }
        }
    }
}

void Atmosphere::ProcessMove(qint32 game_tick)
{
    AssertGrid();

    QElapsedTimer timer;
    timer.start();

    for (int z = 0; z < z_size_; ++z)
    {
        for (int x = 0; x < x_size_; ++x)
        {
            for (int y = 0; y < y_size_; ++y)
            {
                ProcessTileMove(x, y, z, game_tick);
            }
        }
    }
    movement_processing_ns_
            = (movement_processing_ns_ + timer.nsecsElapsed()) / 2;
}

void Atmosphere::Represent(GrowingFrame* frame) const
{
    frame->Append(
        FrameData::TextEntry{
            "Performance",
            QString("Atmos grid processing: %1 ms").arg((grid_processing_ns_ * 1.0) / 1000000.0)});

    frame->Append(
        FrameData::TextEntry{
            "Performance",
            QString("Atmos move processing: %1 ms").arg((movement_processing_ns_ * 1.0) / 1000000.0)});
}

void Atmosphere::SetFlags(quint32 x, quint32 y, quint32 z, AtmosInterface::Flags flags)
{
    AssertGrid();

    grid_->At(x, y).flags = flags;
}

void Atmosphere::LoadGrid(MapInterface* map)
{
    map_ = map;

    Resize(map_->GetWidth(), map_->GetHeight(), map_->GetDepth());

    for (int z = 0; z < z_size_; ++z)
    {
        for (int x = 0; x < x_size_; ++x)
        {
            for (int y = 0; y < y_size_; ++y)
            {
                auto& tile = map_->At(x, y, z);
                tile->UpdateAtmosPassable();
                atmos::AtmosHolder* holder = tile->GetAtmosHolder();
                atmos::AtmosGrid::Cell& cell = grid_->At(x, y);

                cell.data = holder->data_;
                holder->SetAtmosData(&cell.data);
            }
        }
    }
}

void Atmosphere::AssertGrid()
{
    if (!map_)
    {
        kv::Abort("Atmosphere: Grid is not loaded!");
    }
}
