#pragma once

#include <qglobal.h>

enum class Dir : qint32
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

namespace kv
{
namespace helpers
{
    const int DIR_TO_BYOND[10] = {3, 2, 1, 0, 3, 3, 4, 5, 6, 7};

    inline int DirToByond(Dir dir)
    {
        // TODO: switch
        return DIR_TO_BYOND[static_cast<int>(dir)];
    }
}
}
