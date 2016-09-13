#include "AtmosGrid.h"

void AtmosGrid::Process()
{
    Prepare(0);
    Finalize();
}

void AtmosGrid::Prepare(int stage)
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
        for (int column = 1; column < height_ - 1; ++column)
        {

            Cell& current = cells_[pos];

            if (!current.IsPassable(Cell::CENTER))
            {
                // TODO: if something still here
                ++pos;
                continue;
            }

            if (((column + line) % 2) == (MAIN_TICK % 2))
            {
                ++pos;
                continue;
            }

            const int DIRS_SIZE = 4;
            const IAtmosphere::Flags DIRS[DIRS_SIZE]
                = { Cell::LEFT, Cell::UP, Cell::DOWN, Cell::RIGHT };
            const IAtmosphere::Flags REVERT_DIRS[DIRS_SIZE]
                = { Cell::RIGHT, Cell::DOWN, Cell::UP, Cell::LEFT };

            /*const int DIRS_SIZE = 4;

            const IAtmosphere::Flags DIRS_DATA_LR[DIRS_SIZE]
                = { Cell::LEFT, Cell::RIGHT };
            const IAtmosphere::Flags REVERT_DIRS_DATA_LR[DIRS_SIZE]
                = { Cell::RIGHT, Cell::LEFT };

            const IAtmosphere::Flags DIRS_DATA_UD[DIRS_SIZE]
                = { Cell::LEFT, Cell::UP, Cell::DOWN, Cell::RIGHT };
            const IAtmosphere::Flags REVERT_DIRS_DATA_UD[DIRS_SIZE]
                = { Cell::RIGHT, Cell::DOWN, Cell::UP, Cell::LEFT };

            const IAtmosphere::Flags* DIRS;
            const IAtmosphere::Flags* REVERT_DIRS;
            if ((MAIN_TICK / 2) % 2 == 0)
            {
                DIRS = DIRS_DATA_LR;
                REVERT_DIRS = REVERT_DIRS_DATA_LR;
            }
            else
            {
                DIRS = DIRS_DATA_UD;
                REVERT_DIRS = REVERT_DIRS_DATA_UD;
            }*/

            Cell* near_cells[DIRS_SIZE];
            int near_size = 0;
            int gases_sums[GASES_NUM];
            for (int i = 0; i < GASES_NUM; ++i)
            {
                gases_sums[i] = 0;
            }
            int energy_sum = 0;

            // TODO: function which will operate under
            // 5 cells

            for (int dir = 0; dir < DIRS_SIZE; ++dir)
            {
                if (current.IsPassable(DIRS[dir]))
                {
                    Cell& nearby = Get(pos, DIRS[dir]);
                    if (   nearby.IsPassable(REVERT_DIRS[dir])
                        && nearby.IsPassable(Cell::CENTER))
                    {
                        near_cells[dir] = &nearby;
                        for (int i = 0; i < GASES_NUM; ++i)
                        {
                            gases_sums[i] += nearby.data.gases[i];
                        }
                        energy_sum += nearby.data.energy;
                        ++near_size;
                        continue;
                    }
                }
                near_cells[dir] = nullptr;
            }

            for (int i = 0; i < GASES_NUM; ++i)
            {
                gases_sums[i] += current.data.gases[i];
            }
            energy_sum += current.data.energy;

            int gases_average[GASES_NUM];
            for (int i = 0; i < GASES_NUM; ++i)
            {
                gases_average[i] = gases_sums[i] / (near_size + 1);
            }
            int energy_average = energy_sum / (near_size + 1);

            for (int dir = 0; dir < DIRS_SIZE; ++dir)
            {
                if (near_cells[dir])
                {
                    Cell& nearby = *near_cells[dir];
                    for (int i = 0; i < GASES_NUM; ++i)
                    {
                        int diff = gases_average[i] - nearby.data.gases[i];
                        nearby.diffs[i] += diff;
                    }
                    int diff = energy_average - nearby.data.energy;
                    nearby.energy_diff += diff;
                }
            }

            for (int i = 0; i < GASES_NUM; ++i)
            {
                int diff = gases_average[i] - current.data.gases[i];
                current.diffs[i] += diff;
            }
            int diff = energy_average - current.data.energy;
            current.energy_diff += diff;

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

void AtmosGrid::Finalize()
{
    for (int pos = 0; pos < length_; ++pos)
    {
        Cell& cell = cells_[pos];

        for (int i = 0; i < GASES_NUM; ++i)
        {
            cell.data.gases[i] += cell.diffs[i];
            cell.diffs[i] = 0;
        }
        cell.data.energy += cell.energy_diff;
        cell.energy_diff = 0;

        if (cell.flags & AtmosGrid::Cell::SPACE)
        {
            for (int i = 0; i < GASES_NUM; ++i)
            {
                cell.data.gases[i] *= 4;
                cell.data.gases[i] /= 5;
            }
            cell.data.energy *= 4;
            cell.data.energy /= 5;
        }

        UpdateMacroParams(&cell.data);
    }
}
