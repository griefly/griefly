#pragma once

#include <map>
#include <cstdio>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <qglobal.h>
#include <QString>
#include <QDebug>

#include "FastSerializer.h"

const int SIZE_H_SQ = 12; // visible size const
const int SIZE_W_SQ = 12;

extern bool NODRAW; // probably nodraw mode

enum class Dir : int
{
    ALL = -1,
    LEFT = 0,
    RIGHT = 1,
    UP = 2,
    DOWN = 3,
    ZUP = 4,
    ZDOWN = 5,

    SOUTHEAST = 6,
    SOUTHWEST = 7,
    NORTHEAST = 8,
    NORTHWEST = 9,
};

inline FastSerializer& operator<<(FastSerializer& file, const Dir& dir)
{
    file << static_cast<int>(dir);
    return file;
}

inline FastDeserializer& operator>>(FastDeserializer& file, Dir& dir)
{
    int temp;
    file >> temp;
    dir = static_cast<Dir>(temp);
    return file;
}

struct VDir
{
    VDir(int new_x = 0, int new_y = 0, int new_z = 0)
        : x(new_x), y(new_y), z(new_z) { }
    int x;
    int y;
    int z;
};

struct PosPoint
{
    PosPoint(int new_x = 0, int new_y = 0, int new_z = 0)
        : posx(new_x), posy(new_y), posz(new_z) { }
    bool operator==(const PosPoint& other) const
    {
        return (posx == other.posx) && (posy == other.posy) && (posz == other.posz);
    }
    bool operator!=(const PosPoint& other) const
    {
        return !operator==(other);
    }

    int posx;
    int posy;
    int posz;
};

inline bool IsNonZero(const VDir& vdir)
{
    return    vdir.x
           || vdir.y
           || vdir.z;
}

const VDir VD_LEFT(-1, 0, 0); // west
const VDir VD_RIGHT(1, 0, 0); // east
const VDir VD_UP(0, -1, 0); // north
const VDir VD_DOWN(0, 1, 0); // south
const VDir VD_ZUP(0, 0, 1);
const VDir VD_ZDOWN(0, 0, -1);

namespace helpers
{
    const VDir DirToVDir[6] = {VD_LEFT, VD_RIGHT, VD_UP, VD_DOWN, VD_ZUP, VD_ZDOWN};
}
inline VDir DirToVDir(Dir dir)
{
    // TODO: switch
    int index = static_cast<int>(dir);
    return helpers::DirToVDir[index];
}

inline Dir VDirToDir(const VDir& vdir)
{
    int abs_x = std::abs(vdir.x);
    int abs_y = std::abs(vdir.y);
    int abs_z = std::abs(vdir.z);
    if ((abs_x > abs_y) && (abs_x > abs_z))
    {
        if (vdir.x > 0)
        {
            return Dir::RIGHT;
        }
        return Dir::LEFT;
    }
    if (abs_y >= abs_z)
    {
        if (vdir.y >= 0)
        {
            return Dir::DOWN;
        }
        return Dir::UP;
    }

    if (vdir.z > 0)
    {
        return Dir::ZUP;
    }
    return Dir::ZDOWN;
}

inline Dir RevertDir(Dir dir)
{
    switch (dir)
    {
    case Dir::UP:
        return Dir::DOWN;
    case Dir::DOWN:
        return Dir::UP;
    case Dir::LEFT:
        return Dir::RIGHT;
    case Dir::RIGHT:
        return Dir::LEFT;
    case Dir::ZUP:
        return Dir::ZDOWN;
    case Dir::ZDOWN:
        return Dir::ZUP;
    case Dir::NORTHEAST:
        return Dir::SOUTHWEST;
    case Dir::SOUTHWEST:
        return Dir::NORTHEAST;
    case Dir::SOUTHEAST:
        return Dir::NORTHWEST;
    case Dir::NORTHWEST:
        return Dir::SOUTHEAST;
    }
}

const int MAX_LEVEL = 20;

extern quint32 MAIN_TICK;
