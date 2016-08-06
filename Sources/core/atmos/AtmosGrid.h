#pragma once

#include <QDebug>

#include "AtmosHolder.h"
#include "KVAbort.h"

class AtmosGrid
{
public:
    struct Cell
    {
        static const char FULL = 0;
        static const char UP = 1;
        static const char DOWN = 2;
        static const char LEFT = 4;
        static const char RIGHT = 8;
        static const char CENTER = 16;
        static const char SPACE = 32;

        int gases[GASES_NUM];
        int diffs[GASES_NUM];
        int energy;
        char flags;


        Cell()
        {
            for (int i = 0; i < GASES_NUM; ++i)
            {
                gases[i] = 0;
                diffs[i] = 0;
            }
            flags = FULL;
            energy = 0;
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
