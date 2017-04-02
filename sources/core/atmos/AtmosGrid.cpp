#include "AtmosGrid.h"

void AtmosGrid::Process()
{
    ProcessGroups();
    Finalize();
}

inline void ProcessFiveCells(AtmosGrid::Cell* near_cells[])
{
    int near_size = 0;
    int gases_sums[GASES_NUM];
    for (int i = 0; i < GASES_NUM; ++i)
    {
        gases_sums[i] = 0;
    }
    int energy_sum = 0;

    for (int dir = 0; dir < atmos::DIRS_SIZE + 1; ++dir)
    {
        if (AtmosGrid::Cell* nearby = near_cells[dir])
        {
            for (int i = 0; i < GASES_NUM; ++i)
            {
                gases_sums[i] += nearby->data.gases[i];
            }
            energy_sum += nearby->data.energy;
            ++near_size;
        }
    }

    int gases_average[GASES_NUM];
    int gases_remains[GASES_NUM];
    for (int i = 0; i < GASES_NUM; ++i)
    {
        gases_average[i] = gases_sums[i] / near_size;
        gases_remains[i] = gases_sums[i] % near_size;
    }
    int energy_average = energy_sum / near_size;
    int energy_remains = energy_sum % near_size;

    AtmosGrid::Cell& center = *near_cells[atmos::DIRS_SIZE];

    for (int dir = 0; dir < atmos::DIRS_SIZE; ++dir)
    {
        if (near_cells[dir])
        {
            AtmosGrid::Cell& nearby = *near_cells[dir];
            for (int i = 0; i < GASES_NUM; ++i)
            {
                int diff = gases_average[i] - nearby.data.gases[i];
                nearby.flows[atmos::REVERT_DIRS_INDEXES[dir]] += diff;
                center.flows[dir] -= diff;
                nearby.data.gases[i] = gases_average[i];
            }
            nearby.data.energy = energy_average;
        }
    }

    for (int i = 0; i < GASES_NUM; ++i)
    {
        center.data.gases[i] = gases_average[i] + gases_remains[i];
    }
    center.data.energy = energy_average + energy_remains;
}

inline AtmosGrid::Cell& GetNearInGroup(AtmosGrid::Cell* current, IAtmosphere::Flags dir)
{
    switch (dir)
    {
    case atmos::DOWN:  return *(current - 1);
    case atmos::UP:    return *(current + 1);
    case atmos::RIGHT: return *(current + atmos::CELL_GROUP_SIZE);
    case atmos::LEFT:  return *(current - atmos::CELL_GROUP_SIZE);
    default: break;
    }
}

void AtmosGrid::ProcessGroups()
{
    const int AMOUNT_CELLS_IN_GROUP = atmos::CELL_GROUP_SIZE * atmos::CELL_GROUP_SIZE;
    for (int group_index = 0; group_index < group_height_ * group_width_; ++group_index)
    {
        Cell* current_group = &cells_[group_index * AMOUNT_CELLS_IN_GROUP];
        for (int stage = 0; stage < 5; ++stage)
        {
            //int
            for (int x = 1; x < atmos::CELL_GROUP_SIZE - 1; ++x)
            {
                for (int y = 1; y < atmos::CELL_GROUP_SIZE - 1; ++y)
                {
                    if (((x + (y * 2)) % 5) != ((MAIN_TICK + stage) % 5))
                    {
                        continue;
                    }

                    Cell& current = current_group[y + x * atmos::CELL_GROUP_SIZE];

                    if (!current.IsPassable(atmos::CENTER))
                    {
                        continue;
                    }

                    Cell* near_cells[atmos::DIRS_SIZE + 1];

                    for (int dir = 0; dir < atmos::DIRS_SIZE; ++dir)
                    {
                        if (current.IsPassable(atmos::DIRS[dir]))
                        {
                            Cell& nearby = GetNearInGroup(&current, atmos::DIRS[dir]);
                            if (   nearby.IsPassable(atmos::REVERT_DIRS[dir])
                                && nearby.IsPassable(atmos::CENTER))
                            {
                                near_cells[dir] = &nearby;
                                continue;
                            }
                        }
                        near_cells[dir] = nullptr;
                    }
                    near_cells[atmos::DIRS_SIZE] = &current;

                    ProcessFiveCells(near_cells);
                }
            }
        }
    }
}

void AtmosGrid::Finalize()
{
    for (int pos = 0; pos < length_; ++pos)
    {
        Cell& cell = cells_[pos];

        if (cell.flags & atmos::SPACE)
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
