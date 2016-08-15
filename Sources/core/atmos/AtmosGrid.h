#pragma once

#include <QDebug>

#include "AtmosConstants.h"
#include "KVAbort.h"
#include "Interfaces.h"

class AtmosGrid
{
public:
    struct Cell
    {
        static const IAtmosphere::Flags FULL = 0;
        static const IAtmosphere::Flags UP = 1;
        static const IAtmosphere::Flags DOWN = 2;
        static const IAtmosphere::Flags LEFT = 4;
        static const IAtmosphere::Flags RIGHT = 8;
        static const IAtmosphere::Flags CENTER = 16;
        static const IAtmosphere::Flags SPACE = 32;

        AtmosData data;

        int diffs[GASES_NUM];
        unsigned int energy_diff;
        IAtmosphere::Flags flags;
        Cell()
        {
            for (int i = 0; i < GASES_NUM; ++i)
            {
                data.gases[i] = 0;
                diffs[i] = 0;
            }
            flags = FULL;
            data.energy = 0;
            energy_diff = 0;
            data.pressure = 0;
            data.temperature = 0;
            data.volume = 1;
        }
        char IsPassable(char dir)
        {
            return ~flags & dir;
        }
        void SetUnpassable(char dir)
        {
            flags |= dir;
        }
        void ResetPassable()
        {
            flags = FULL;
        }
    };
    AtmosGrid(int width, int height)
        : width_(width),
          height_(height),
          length_(width_ * height_)
    {
        if (width_ < 2)
        {
            qDebug() << "Width too small for AtmosGrid!";
            kv_abort();
        }
        if (height_ < 2)
        {
            qDebug() << "Height too small for AtmosGrid!";
            kv_abort();
        }
        cells_ = new Cell[length_];
    }
    ~AtmosGrid()
    {
        delete[] cells_;
    }
    Cell& At(int x, int y)
    {
        return cells_[x + y * width_];
    }
    void Process();

private:
    int width_;
    int height_;
    int length_;
    Cell* cells_;
};
