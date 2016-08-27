#include "AtmosGrid.h"

void AtmosGrid::Process()
{
    int pos = 0;

    // Left border line
    for (; pos < height_; ++pos)
    {
        Cell& current = cells_[pos];
        current.Truncate();
    }

    for (int line = 1; line < width_ - 1; ++line)
    {
        // Upper border line
        {
            Cell& current = cells_[pos];
            current.Truncate();
        }
        ++pos;
        for (int i = 1; i < height_ - 1; ++i)
        {
            Cell& current = cells_[pos];

            if (current.flags & Cell::SPACE)
            {
                current.Truncate();
            }

            if (!current.IsPassable(Cell::CENTER))
            {
                // TODO: if something still here
                ++pos;
                continue;
            }

            // TODO: not 5 tiles as whole
            // TODO: use diffs

            const int DIRS_SIZE = 4;
            const IAtmosphere::Flags DIRS[DIRS_SIZE]
                = { Cell::LEFT, Cell::UP, Cell::DOWN, Cell::RIGHT };
            const IAtmosphere::Flags REVERT_DIRS[DIRS_SIZE]
                = { Cell::RIGHT, Cell::DOWN, Cell::UP, Cell::LEFT };

            for (int dir = 0; dir < DIRS_SIZE; ++dir)
            {
                if (current.IsPassable(DIRS[dir]))
                {
                    Cell& nearby = Get(pos, DIRS[dir]);
                    if (   nearby.IsPassable(REVERT_DIRS[dir])
                        && nearby.IsPassable(Cell::CENTER))
                    {
                        for (int gas = 0; gas < GASES_NUM; ++gas)
                        {
                            int sum = current.data.gases[gas] + nearby.data.gases[gas];
                            current.data.gases[gas] = sum / 2;
                            nearby.data.gases[gas] = sum - current.data.gases[gas];
                        }

                        int sum = current.data.energy + nearby.data.energy;
                        current.data.energy = sum / 2;
                        nearby.data.energy = sum - current.data.energy;
                    }
                }
            }
            ++pos;
        }
        // Lower border line
        {
            Cell& current = cells_[pos];
            current.Truncate();
        }
        ++pos;
    }

    // Right border line
    for (int i = 0; i < height_; ++i, ++pos)
    {
        Cell& current = cells_[pos];
        current.Truncate();
    }
}
