#pragma once

#include <QDebug>

#include "AtmosConstants.h"
#include "core/KvAbort.h"
#include "Interfaces.h"

namespace atmos
{
    const int DIRS_SIZE = 4;

    const AtmosInterface::Flags CLEAR = 0;
    const AtmosInterface::Flags UP_BLOCK = 1;
    const AtmosInterface::Flags DOWN_BLOCK = 2;
    const AtmosInterface::Flags LEFT_BLOCK = 4;
    const AtmosInterface::Flags RIGHT_BLOCK = 8;
    const AtmosInterface::Flags CENTER_BLOCK = 16;
    const AtmosInterface::Flags SPACE_TILE = 32;
    const AtmosInterface::Flags NO_OBJECTS = 64;

    const AtmosInterface::Flags DIRS[DIRS_SIZE]
        = { LEFT_BLOCK, UP_BLOCK, DOWN_BLOCK, RIGHT_BLOCK };
    const AtmosInterface::Flags REVERT_DIRS[DIRS_SIZE]
        = { RIGHT_BLOCK, DOWN_BLOCK, UP_BLOCK, LEFT_BLOCK };
    const int REVERT_DIRS_INDEXES[DIRS_SIZE]
        = { 3, 2, 1, 0 };

    const Dir INDEXES_TO_DIRS[DIRS_SIZE]
        = { Dir::WEST, Dir::NORTH, Dir::SOUTH, Dir::EAST };
    const int CELL_GROUP_SIZE = 32;

    class AtmosGrid
    {
    public:
        struct Cell
        {
            AtmosData data;

            int flows[atmos::DIRS_SIZE];

            AtmosInterface::Flags flags;
            Cell()
            {
                for (int i = 0; i < GASES_NUM; ++i)
                {
                    data.gases[i] = 0;
                }
                for (int i = 0; i < atmos::DIRS_SIZE; ++i)
                {
                    flows[i] = 0;
                }
                flags = atmos::CLEAR;
                data.energy = 0;
                data.pressure = 0;
                data.temperature = 0;
                data.volume = 1;
            }
            inline char IsPassable(char dir)
            {
                return ~flags & dir;
            }
            inline void SetUnpassable(char dir)
            {
                flags |= dir;
            }
            inline void ResetPassable()
            {
                flags = atmos::CLEAR;
            }
            inline void Truncate()
            {
                for (int gas = 0; gas < GASES_NUM; ++gas)
                {
                    data.gases[gas] = 0;
                }
                data.energy = 0;
            }
        };
        AtmosGrid(int width, int height)
            : width_(width),
              height_(height),
              length_(width_ * height_),
              group_height_(height / atmos::CELL_GROUP_SIZE),
              group_width_(width / atmos::CELL_GROUP_SIZE)
        {
            if (width_ < 2)
            {
                kv::Abort("Width too small for AtmosGrid!");
            }
            if (height_ < 2)
            {
                kv::Abort("Height too small for AtmosGrid!");
            }
            if ((width_ % atmos::CELL_GROUP_SIZE) != 0)
            {
                kv::Abort(QString("Width is not multiplier of %1!").arg(atmos::CELL_GROUP_SIZE));
            }
            if ((height_ % atmos::CELL_GROUP_SIZE) != 0)
            {
                kv::Abort(QString("Height is not multiplier of %1!").arg(atmos::CELL_GROUP_SIZE));
            }
            cells_ = new Cell[length_];
        }
        ~AtmosGrid()
        {
            delete[] cells_;
        }
        inline Cell& Get(int x, int y, Dir dir)
        {
            switch (dir)
            {
            case Dir::SOUTH:
                return At(x, y + 1);
            case Dir::NORTH:
                return At(x, y - 1);
            case Dir::EAST:
                return At(x + 1, y);
            case Dir::WEST:
                return At(x - 1, y);
            }
            KV_UNREACHABLE
        }

        inline Cell& At(int x, int y)
        {
            int group_x = x / atmos::CELL_GROUP_SIZE;
            int x_in_group = x % atmos::CELL_GROUP_SIZE;
            int group_y = y / atmos::CELL_GROUP_SIZE;
            int y_in_group = y % atmos::CELL_GROUP_SIZE;
            Cell* group = &(cells_[(group_y + group_x * group_height_) * atmos::CELL_GROUP_SIZE * atmos::CELL_GROUP_SIZE]);
            return group[y_in_group + x_in_group * atmos::CELL_GROUP_SIZE];
        }
        void Process(qint32 game_tick);
    private:
        void ProcessGroups(qint32 game_tick);
        void ProcessGroupsBorders(qint32 game_tick);
        void Finalize();

        void ProcessBorderGroupCell(Cell* current, int x, int y);

        int width_;
        int height_;
        int group_height_;
        int group_width_;
        int length_;
        Cell* cells_;
    };
}
