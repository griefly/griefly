#include "Atmos.h"

#include <algorithm>

#include "SyncRandom.h"
#include "Map.h"
#include "Helpers.h"

Atmosphere::Atmosphere(SyncRandom* random, MapMaster* map)
    : random_(random),
      map_(map)
{
    qDebug() << "Atmosphere load";
}

void Atmosphere::Resize(size_t x, size_t y)
{
    x_shuffle_.resize(x);
    y_shuffle_.resize(y);

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
}

void Atmosphere::Process()
{
    for (size_t z_counter = 0; z_counter < static_cast<size_t>(map_->GetMapD()); ++z_counter)
    {
        ShuffleX();
        ShuffleY();
        ShuffleDir();
        for (size_t x_sh = 0; x_sh < x_shuffle_.size(); ++x_sh)
        {
            size_t x_counter = x_shuffle_[x_sh];
            for (size_t y_sh = 0; y_sh < y_shuffle_.size(); ++y_sh)
            {
                size_t y_counter = y_shuffle_[y_sh];

                ProcessTile(x_counter, y_counter, z_counter);
            }
        }
    }
}

void Atmosphere::ProcessTile(size_t x, size_t y, size_t z)
{
    auto tile = map_->squares[x][y][z];
    
    if (tile->GetTurf()->GetAtmosState() == NON_SIMULATED)
    {
        return;
    }

    //ShuffleDir();

    for (size_t d_sh = 0; d_sh < dir_shuffle_.size(); ++d_sh)
    {
        Dir dir = dir_shuffle_[d_sh];
        auto neighbour = tile->GetNeighbourImpl(dir);
        if (!(   CanPass(tile->GetPassable(dir), Passable::AIR)
              && CanPass(neighbour->GetPassable(helpers::revert_dir(dir)), Passable::AIR)
              && CanPass(neighbour->GetPassable(D_ALL), Passable::AIR))
            )
        {
            continue;
        }

        if (neighbour->GetTurf()->GetAtmosState() == NON_SIMULATED)
        {
            continue;
        }

        int p = MAX_GAS_LEVEL / 2;
        if (!CanPass(tile->GetPassable(D_ALL), Passable::AIR))
        {
            p = 0;
        }

        tile->GetAtmosHolder()->Connect(
            neighbour->GetAtmosHolder(), 
            MAX_GAS_LEVEL, MAX_GAS_LEVEL, p);

        if (tile->GetTurf()->GetAtmosState() == SPACE)
        {
            tile->GetAtmosHolder()->Truncate();
        }
        if (neighbour->GetTurf()->GetAtmosState() == SPACE)
        {
            neighbour->GetAtmosHolder()->Truncate();
        }
    }
}

void Atmosphere::ProcessMove()
{
    for (size_t z_counter = 0; z_counter < static_cast<size_t>(map_->GetMapD()); ++z_counter)
    {
        ShuffleX();
        ShuffleY();
        ShuffleDir();
        for (size_t x_sh = 0; x_sh < x_shuffle_.size(); ++x_sh)
        {
            size_t x_counter = x_shuffle_[x_sh];
            for (size_t y_sh = 0; y_sh < y_shuffle_.size(); ++y_sh)
            {
                size_t y_counter = y_shuffle_[y_sh];

                ProcessTileMove(x_counter, y_counter, z_counter);
            }
        }
    }
}

const unsigned int PRESSURE_MOVE_BORDER = 1000;

void Atmosphere::ProcessTileMove(size_t x, size_t y, size_t z)
{
    auto tile = map_->squares[x][y][z];
    
    if (tile->GetTurf()->GetAtmosState() == NON_SIMULATED)
    {
        return;
    }
    if (tile->GetTurf()->GetAtmosState() == SPACE)
    {
        return;
    }

    for (size_t d_sh = 0; d_sh < dir_shuffle_.size(); ++d_sh)
    {
        Dir dir = dir_shuffle_[d_sh];

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
