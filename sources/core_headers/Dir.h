#pragma once

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
