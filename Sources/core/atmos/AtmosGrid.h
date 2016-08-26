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
            flags = FULL;
        }
        inline void Truncate()
        {
            for (int gas = 0; gas < GASES_NUM; ++gas)
            {
                data.gases[gas] = 0;
                data.energy = 0;
                data.pressure = 0;
            }
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

    inline Cell& Get(int pos, IAtmosphere::Flags dir)
    {
        // TODO: Boundaries check?

        switch (dir)
        {
        case Cell::DOWN:  return cells_[pos + 1];
        case Cell::UP:    return cells_[pos - 1];
        case Cell::RIGHT: return cells_[pos + width_];
        case Cell::LEFT:  return cells_[pos - width_];
        default: break;
        }

        qDebug() << "AtmosGrid::Get, Unknown dir:" << dir;
        kv_abort();
        // Not reachable
        return cells_[pos];
    }
    inline Cell& At(int x, int y)
    {
        return cells_[y + x * height_];
    }
    void Process();

private:
    int width_;
    int height_;
    int length_;
    Cell* cells_;
};
