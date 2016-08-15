#include "AtmosGrid.h"

void AtmosGrid::Process()
{
    // TODO: upper line
    // TODO: check order (height_/width_ mismatch), because
    // map looks like map[x][y]
    int pos = width_;
    for (int line = 1; line < height_ - 1; ++line)
    {
        // TODO: left line
        ++pos;
        for (int i = 1; i < width_ - 1; ++i)
        {
            // UP
            Cell& current = cells_[pos];

            if (current.flags & Cell::SPACE)
            {
                for (int gas = 0; gas < GASES_NUM; ++gas)
                {
                    current.data.gases[gas] = 0;
                    current.data.energy = 0;
                    current.data.pressure = 0;
                }
            }

            if (!current.IsPassable(Cell::CENTER))
            {
                // TODO: if something still here
                ++pos;
                continue;
            }

            // TODO: not 5 tiles as whole
            // TODO: use diffs
            if (current.IsPassable(Cell::UP))
            {
                Cell& nearby = cells_[pos - 1];
                if (   nearby.IsPassable(Cell::DOWN)
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
            if (current.IsPassable(Cell::LEFT))
            {
                Cell& nearby = cells_[pos - width_];
                if (   nearby.IsPassable(Cell::RIGHT)
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
            if (current.IsPassable(Cell::RIGHT))
            {
                Cell& nearby = cells_[pos + width_];
                if (   nearby.IsPassable(Cell::LEFT)
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
            if (current.IsPassable(Cell::DOWN))
            {
                Cell& nearby = cells_[pos + 1];
                if (   nearby.IsPassable(Cell::UP)
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

            ++pos;
        }
        // TODO: right line
        ++pos;
    }
    // TODO: lower line
}
