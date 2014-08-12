#include "Atmos.h"

#include <algorithm>

#include "sync_random.h"
#include "MapClass.h"
#include "helpers.h"

void Atmosphere::Process()
{
    for (size_t z_counter = 0; z_counter < static_cast<size_t>(GetMapMaster()->GetMapD()); ++z_counter)
    {
        ShuffleX();
        ShuffleY();
        
        for (size_t x_sh = 0; x_sh < x_shuffle_.size(); ++x_sh)
        {
            size_t x_counter = x_shuffle_[x_sh];
            if (x_sh % 10 == 1)
                ShuffleDir();
            //ShuffleY();
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
    auto tile = GetMapMaster()->squares[x][y][z];
    
    if (tile->GetTurf()->GetAtmosState() == NON_SIMULATED)
        return;

    //ShuffleDir();

    for (size_t d_sh = 0; d_sh < dir_shuffle_.size(); ++d_sh)
    {
        Dir dir = dir_shuffle_[d_sh];
        auto neighbour = tile->GetNeighbourImpl(dir);
        if (!(   CanPass(tile->GetPassable(dir), Passable::AIR)
              && CanPass(neighbour->GetPassable(helpers::revert_dir(dir)), Passable::AIR)
              && CanPass(neighbour->GetPassable(D_ALL), Passable::AIR))
            )
            continue;

        if (neighbour->GetTurf()->GetAtmosState() == NON_SIMULATED)
            continue;

        int p = MAX_GAS_LEVEL / 2;
        if (!CanPass(tile->GetTurf()->GetPassable(D_ALL), Passable::AIR))
            p = 0;

        tile->GetAtmosHolder()->Connect(
            neighbour->GetAtmosHolder(), 
            MAX_GAS_LEVEL, MAX_GAS_LEVEL, p);

        if (tile->GetTurf()->GetAtmosState() == SPACE)
            tile->GetAtmosHolder()->Truncate();
        if (neighbour->GetTurf()->GetAtmosState() == SPACE)
            neighbour->GetAtmosHolder()->Truncate();
    }

}

void Atmosphere::ShuffleX()
{
    std::random_shuffle(x_shuffle_.begin(), x_shuffle_.end(), random_helpers::random_shuffle);
}

void Atmosphere::ShuffleY()
{
   std::random_shuffle(y_shuffle_.begin(), y_shuffle_.end(), random_helpers::random_shuffle);
}

void Atmosphere::ShuffleDir()
{
   std::random_shuffle(dir_shuffle_.begin(), dir_shuffle_.end(), random_helpers::random_shuffle);
}