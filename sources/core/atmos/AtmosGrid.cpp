#include "AtmosGrid.h"

namespace atmos
{

void AtmosGrid::Process(qint32 game_tick)
{
    ProcessGroups(game_tick);
    ProcessGroupsBorders(game_tick);
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

inline AtmosGrid::Cell& GetNearInGroup(AtmosGrid::Cell* current, Dir dir)
{
    switch (dir)
    {
    case Dir::SOUTH:
        return *(current + 1);
    case Dir::NORTH:
        return *(current - 1);
    case Dir::EAST:
        return *(current + atmos::CELL_GROUP_SIZE);
    case Dir::WEST:
        return *(current - atmos::CELL_GROUP_SIZE);
    }
    KV_UNREACHABLE
}

namespace
{
    const int AMOUNT_CELLS_IN_GROUP = atmos::CELL_GROUP_SIZE * atmos::CELL_GROUP_SIZE;
    const int STAGES_AMOUNT = 5;
    inline bool BelongsToStage(int x, int y, int stage, qint32 game_tick)
    {
        return (((x + (y * 2)) % STAGES_AMOUNT) == ((game_tick + stage) % STAGES_AMOUNT));
    }

    inline void ProcessInnerGroupCell(AtmosGrid::Cell* current)
    {
        if (!current->IsPassable(atmos::CENTER_BLOCK))
        {
            return;
        }

        AtmosGrid::Cell* near_cells[atmos::DIRS_SIZE + 1];

        for (int dir = 0; dir < atmos::DIRS_SIZE; ++dir)
        {
            if (current->IsPassable(atmos::DIRS[dir]))
            {
                AtmosGrid::Cell& nearby = GetNearInGroup(current, atmos::INDEXES_TO_DIRS[dir]);
                if (   nearby.IsPassable(atmos::REVERT_DIRS[dir])
                    && nearby.IsPassable(atmos::CENTER_BLOCK))
                {
                    near_cells[dir] = &nearby;
                    continue;
                }
            }
            near_cells[dir] = nullptr;
        }
        near_cells[atmos::DIRS_SIZE] = current;

        ProcessFiveCells(near_cells);
    }
}
inline void AtmosGrid::ProcessBorderGroupCell(Cell* current, int x, int y)
{
    if (!current->IsPassable(atmos::CENTER_BLOCK))
    {
        return;
    }

    Cell* near_cells[atmos::DIRS_SIZE + 1];

    for (int dir = 0; dir < atmos::DIRS_SIZE; ++dir)
    {
        if (current->IsPassable(atmos::DIRS[dir]))
        {
            Cell& nearby = Get(x, y, atmos::INDEXES_TO_DIRS[dir]);
            if (   nearby.IsPassable(atmos::REVERT_DIRS[dir])
                && nearby.IsPassable(atmos::CENTER_BLOCK))
            {
                near_cells[dir] = &nearby;
                continue;
            }
        }
        near_cells[dir] = nullptr;
    }
    near_cells[atmos::DIRS_SIZE] = current;

    ProcessFiveCells(near_cells);
}

void AtmosGrid::ProcessGroups(qint32 game_tick)
{
    Cell* current_group = &cells_[0];
    const int GROUPS_AMOUNT = group_height_ * group_width_;
    for (int index = 0; index < GROUPS_AMOUNT; ++index, current_group += AMOUNT_CELLS_IN_GROUP)
    {
        for (int stage = 0; stage < STAGES_AMOUNT; ++stage)
        {
            for (int x = 1; x < atmos::CELL_GROUP_SIZE - 1; ++x)
            {
                for (int y = 1; y < atmos::CELL_GROUP_SIZE - 1; ++y)
                {
                    if (!BelongsToStage(x, y, stage, game_tick))
                    {
                        continue;
                    }
                    Cell& current = current_group[y + x * atmos::CELL_GROUP_SIZE];
                    ProcessInnerGroupCell(&current);
                }
            }
        }
    }
}

void AtmosGrid::ProcessGroupsBorders(qint32 game_tick)
{
    for (int group_x = 1; group_x < group_width_ - 1; ++group_x)
    {
        int end_x = group_x * atmos::CELL_GROUP_SIZE;
        int start_x = end_x - 1;
        for (int stage = 0; stage < STAGES_AMOUNT; ++stage)
        {
            for (int x = start_x; x <= end_x; ++x)
            {
                for (int y = 1; y < height_ - 1; ++y)
                {
                    if (!BelongsToStage(x, y, stage, game_tick))
                    {
                        continue;
                    }
                    Cell& current = At(x, y);
                    ProcessBorderGroupCell(&current, x, y);
                }
            }
        }
    }
    for (int group_y = 1; group_y < group_height_ - 1; ++group_y)
    {
        int end_y = group_y * atmos::CELL_GROUP_SIZE;
        int start_y = end_y - 1;
        for (int stage = 0; stage < STAGES_AMOUNT; ++stage)
        {
            for (int y = start_y; y <= end_y; ++y)
            {
                for (int x = 1; x < width_ - 1; ++x)
                {
                    if (!BelongsToStage(x, y, stage, game_tick))
                    {
                        continue;
                    }
                    Cell& current = At(x, y);
                    ProcessBorderGroupCell(&current, x, y);
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

        if (cell.flags & atmos::SPACE_TILE)
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

}
