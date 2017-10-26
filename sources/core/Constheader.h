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

#include "core_headers/FastSerializer.h"
#include "Hashes.h"

#ifdef Q_CC_GNU
#define KV_UNREACHABLE __builtin_unreachable();
#else
#define KV_UNREACHABLE
#endif // Q_CC_GNU

const int SIZE_H_SQ = 12; // visible size const
const int SIZE_W_SQ = 12;

extern bool NODRAW; // probably nodraw mode

enum class Dir : int
{
    ALL = -1, // Basically, the center of something
    WEST = 0,
    EAST = 1,
    NORTH = 2,
    SOUTH = 3,
    UP = 4,
    DOWN = 5,

    SOUTHEAST = 6,
    SOUTHWEST = 7,
    NORTHEAST = 8,
    NORTHWEST = 9,
};

inline kv::FastSerializer& operator<<(kv::FastSerializer& file, const Dir& dir)
{
    file << static_cast<int>(dir);
    return file;
}

inline kv::FastDeserializer& operator>>(kv::FastDeserializer& file, Dir& dir)
{
    int temp;
    file >> temp;
    dir = static_cast<Dir>(temp);
    return file;
}

inline unsigned int Hash(Dir dir)
{
    return kv::Hash(static_cast<int>(dir));
}

namespace kv
{

struct Vector
{
    Vector() : Vector(0, 0, 0) { }
    Vector(int new_x, int new_y, int new_z)
        : x(new_x), y(new_y), z(new_z) { }
    int x;
    int y;
    int z;
};

inline FastDeserializer& operator>>(FastDeserializer& file, Vector& vdir)
{
    file >> vdir.x;
    file >> vdir.y;
    file >> vdir.z;
    return file;
}

inline FastSerializer& operator<<(FastSerializer& file, const Vector& vdir)
{
    file << vdir.x;
    file << vdir.y;
    file << vdir.z;
    return file;
}

struct Position
{
    Position() : Position(0, 0, 0) { }
    Position(int new_x, int new_y, int new_z)
        : x(new_x), y(new_y), z(new_z) { }
    bool operator==(const Position& other) const
    {
        return (x == other.x) && (y == other.y) && (z == other.z);
    }
    bool operator!=(const Position& other) const
    {
        return !operator==(other);
    }

    int x;
    int y;
    int z;
};

inline FastDeserializer& operator>>(FastDeserializer& file, kv::Position& position)
{
    file >> position.x;
    file >> position.y;
    file >> position.z;
    return file;
}

inline FastSerializer& operator<<(FastSerializer& file, const kv::Position& position)
{
    file << position.x;
    file << position.y;
    file << position.z;
    return file;
}

inline uint qHash(const kv::Position& point, uint seed = 0)
{
    uint retval = 59;
    retval = retval * 13 + ::qHash(point.z, seed);
    retval = retval * 13 + ::qHash(point.x, seed);
    retval = retval * 13 + ::qHash(point.y, seed);
    return retval;
}

inline unsigned int Hash(const Vector& vdir)
{
    return    (vdir.x + 1)
           + ((vdir.y + 1) << 8)
           + ((vdir.z + 1) << 16);
}

inline unsigned int Hash(const Position& position)
{
    return    (position.x + 1)
           + ((position.y + 1) << 8)
           + ((position.z + 1) << 16);
}

}

inline bool IsNonZero(const kv::Vector& vdir)
{
    return    vdir.x
           || vdir.y
           || vdir.z;
}

const kv::Vector VD_LEFT(-1, 0, 0); // west
const kv::Vector VD_RIGHT(1, 0, 0); // east
const kv::Vector VD_UP(0, -1, 0); // north
const kv::Vector VD_DOWN(0, 1, 0); // south
const kv::Vector VD_ZUP(0, 0, 1);
const kv::Vector VD_ZDOWN(0, 0, -1);

namespace helpers
{
    const kv::Vector DirToVDir[6] = {VD_LEFT, VD_RIGHT, VD_UP, VD_DOWN, VD_ZUP, VD_ZDOWN};
}
inline kv::Vector DirToVDir(Dir dir)
{
    // TODO: switch
    int index = static_cast<int>(dir);
    return helpers::DirToVDir[index];
}

inline Dir VDirToDir(const kv::Vector& vdir)
{
    int abs_x = std::abs(vdir.x);
    int abs_y = std::abs(vdir.y);
    int abs_z = std::abs(vdir.z);
    if ((abs_x > abs_y) && (abs_x > abs_z))
    {
        if (vdir.x > 0)
        {
            return Dir::EAST;
        }
        return Dir::WEST;
    }
    if (abs_y >= abs_z)
    {
        if (vdir.y >= 0)
        {
            return Dir::SOUTH;
        }
        return Dir::NORTH;
    }

    if (vdir.z > 0)
    {
        return Dir::UP;
    }
    return Dir::DOWN;
}

inline Dir RevertDir(Dir dir)
{
    switch (dir)
    {
    case Dir::NORTH:
        return Dir::SOUTH;
    case Dir::SOUTH:
        return Dir::NORTH;
    case Dir::WEST:
        return Dir::EAST;
    case Dir::EAST:
        return Dir::WEST;
    case Dir::UP:
        return Dir::DOWN;
    case Dir::DOWN:
        return Dir::UP;
    case Dir::NORTHEAST:
        return Dir::SOUTHWEST;
    case Dir::SOUTHWEST:
        return Dir::NORTHEAST;
    case Dir::SOUTHEAST:
        return Dir::NORTHWEST;
    case Dir::NORTHWEST:
        return Dir::SOUTHEAST;
    }
    KV_UNREACHABLE
}
