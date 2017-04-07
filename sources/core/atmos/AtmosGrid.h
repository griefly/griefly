#pragma once

#include <QDebug>

#include "AtmosConstants.h"
#include "KvAbort.h"
#include "Interfaces.h"

namespace atmos
{
    const int DIRS_SIZE = 4;

    const IAtmosphere::Flags FULL = 0;
    const IAtmosphere::Flags UP = 1;
    const IAtmosphere::Flags DOWN = 2;
    const IAtmosphere::Flags LEFT = 4;
    const IAtmosphere::Flags RIGHT = 8;
    const IAtmosphere::Flags CENTER = 16;
    const IAtmosphere::Flags SPACE = 32;
    const IAtmosphere::Flags EMPTY = 64;

    const IAtmosphere::Flags DIRS[DIRS_SIZE]
        = { LEFT, UP, DOWN, RIGHT };
    const IAtmosphere::Flags REVERT_DIRS[DIRS_SIZE]
        = { RIGHT, DOWN, UP, LEFT };
    const int REVERT_DIRS_INDEXES[DIRS_SIZE]
        = { 3, 2, 1, 0 };

    const Dir INDEXES_TO_DIRS[DIRS_SIZE]
        = { D_LEFT, D_UP, D_DOWN, D_RIGHT };
    const int CELL_GROUP_SIZE = 32;
}

class AtmosGrid
{
public:
    struct Cell
    {
        AtmosData data alignas(16);

        int flows[atmos::DIRS_SIZE] alignas(16);

        IAtmosphere::Flags flags alignas(16);
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
            flags = atmos::FULL;
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
            flags = atmos::FULL;
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
            KvAbort("Width too small for AtmosGrid!");
        }
        if (height_ < 2)
        {
            KvAbort("Height too small for AtmosGrid!");
        }
        if ((width_ % atmos::CELL_GROUP_SIZE) != 0)
        {
            KvAbort(QString("Width is not multiplier of %1!").arg(atmos::CELL_GROUP_SIZE));
        }
        if ((height_ % atmos::CELL_GROUP_SIZE) != 0)
        {
            KvAbort(QString("Height is not multiplier of %1!").arg(atmos::CELL_GROUP_SIZE));
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
        case D_DOWN:
            return At(x, y + 1);
        case D_UP:
            return At(x, y - 1);
        case D_RIGHT:
            return At(x + 1, y);
        case D_LEFT:
            return At(x - 1, y);
        }
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
    void Process();
private:
    void ProcessGroups();
    void ProcessGroupsBorders();
    void Finalize();

    void ProcessBorderGroupCell(Cell* current, int x, int y);

    int width_;
    int height_;
    int group_height_;
    int group_width_;
    int length_;
    Cell* cells_;
};
