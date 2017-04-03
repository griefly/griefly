#include "AtmosGrid.h"

void AtmosGrid::Process()
{
    Prepare();
//    Prepare();

    Finalize();
}

inline void ProcessTwoVerticalCells(AtmosGrid::Cell* top, AtmosGrid::Cell* bot)
{
    int gases_sums[GASES_NUM];
    int energy_sum;
    for (int i = 0; i < GASES_NUM; ++i)
    {
        gases_sums[i] = top->data.gases[i] + bot->data.gases[i];
    }
    energy_sum = top->data.energy + bot->data.energy;

    for (int i = 0; i < GASES_NUM; ++i)
    {
        int average = gases_sums[i] / 2;
        int remains = gases_sums[i] % 2;
        int diff = average - bot->data.gases[i];
        top->flows[atmos::DOWN_INDEX] -= diff;
        bot->flows[atmos::UP_INDEX] += diff;

        top->data.gases[i] = average + remains;
        bot->data.gases[i] = average;
    }
    int energy_average = energy_sum / 2;
    int energy_remains = energy_sum % 2;
    top->data.energy = energy_average + energy_remains;
    bot->data.energy = energy_average;
}

inline void ProcessTwoHorizontalCells(AtmosGrid::Cell* left, AtmosGrid::Cell* right)
{
    int gases_sums[GASES_NUM];
    int energy_sum;
    for (int i = 0; i < GASES_NUM; ++i)
    {
        gases_sums[i] = left->data.gases[i] + right->data.gases[i];
    }
    energy_sum = left->data.energy + right->data.energy;

    for (int i = 0; i < GASES_NUM; ++i)
    {
        int average = gases_sums[i] / 2;
        int remains = gases_sums[i] % 2;
        int diff = average - right->data.gases[i];
        left->flows[atmos::RIGHT_INDEX] -= diff;
        right->flows[atmos::LEFT_INDEX] += diff;

        left->data.gases[i] = average + remains;
        right->data.gases[i] = average;
    }
    int energy_average = energy_sum / 2;
    int energy_remains = energy_sum % 2;
    left->data.energy = energy_average + energy_remains;
    right->data.energy = energy_average;
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

void AtmosGrid::Prepare()
{
    int pos = 0;
    for (int line = 0; line < width_; ++line)
    {
        for (int column = 0; column < height_; ++column, ++pos)
        {
            if (   (column % 2) != 0
                || (line % 2) != 0)
            {
                continue;
            }

            Cell& top_left = cells_[pos];
            Cell& bot_left = cells_[pos + 1];
            Cell& top_right = cells_[pos + width_];
            Cell& bot_right = cells_[pos + 1 + width_];

            if (   top_left.IsPassable(atmos::CENTER)
                && top_left.IsPassable(atmos::RIGHT)
                && top_right.IsPassable(atmos::CENTER)
                && top_right.IsPassable(atmos::LEFT))
            {
                ProcessTwoHorizontalCells(&top_left, &top_right);
            }
            if (   bot_left.IsPassable(atmos::CENTER)
                && bot_left.IsPassable(atmos::RIGHT)
                && bot_right.IsPassable(atmos::CENTER)
                && bot_right.IsPassable(atmos::LEFT))
            {
                ProcessTwoHorizontalCells(&bot_left, &bot_right);
            }
            if (   top_left.IsPassable(atmos::CENTER)
                && top_left.IsPassable(atmos::DOWN)
                && bot_left.IsPassable(atmos::CENTER)
                && bot_left.IsPassable(atmos::UP))
            {
                ProcessTwoVerticalCells(&top_left, &bot_left);
            }
            if (   top_right.IsPassable(atmos::CENTER)
                && top_right.IsPassable(atmos::DOWN)
                && bot_right.IsPassable(atmos::CENTER)
                && bot_right.IsPassable(atmos::UP))
            {
                ProcessTwoVerticalCells(&top_right, &bot_right);
            }
        }
    }
    pos = 0;
    for (int line = 0; line < width_ - 1; ++line)
    {
        for (int column = 0; column < height_ - 1; ++column, ++pos)
        {
            if (   (column % 2) != 1
                || (line % 2) != 1)
            {
                continue;
            }

            Cell& top_left = cells_[pos];
            Cell& bot_left = cells_[pos + 1];
            Cell& top_right = cells_[pos + width_];
            Cell& bot_right = cells_[pos + 1 + width_];

            if (   top_left.IsPassable(atmos::CENTER)
                && top_left.IsPassable(atmos::RIGHT)
                && top_right.IsPassable(atmos::CENTER)
                && top_right.IsPassable(atmos::LEFT))
            {
                ProcessTwoHorizontalCells(&top_left, &top_right);
            }
            if (   bot_left.IsPassable(atmos::CENTER)
                && bot_left.IsPassable(atmos::RIGHT)
                && bot_right.IsPassable(atmos::CENTER)
                && bot_right.IsPassable(atmos::LEFT))
            {
                ProcessTwoHorizontalCells(&bot_left, &bot_right);
            }
            if (   top_left.IsPassable(atmos::CENTER)
                && top_left.IsPassable(atmos::DOWN)
                && bot_left.IsPassable(atmos::CENTER)
                && bot_left.IsPassable(atmos::UP))
            {
                ProcessTwoVerticalCells(&top_left, &bot_left);
            }
            if (   top_right.IsPassable(atmos::CENTER)
                && top_right.IsPassable(atmos::DOWN)
                && bot_right.IsPassable(atmos::CENTER)
                && bot_right.IsPassable(atmos::UP))
            {
                ProcessTwoVerticalCells(&top_right, &bot_right);
            }
        }
        ++pos;
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
