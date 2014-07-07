#pragma once

#include <assert.h>

#include "constheader.h"
#include "MapClass.h"

namespace helpers
{
    inline void move_to_dir(Dir dir, int* x, int* y, int* z = nullptr)
    {
        if (x)
        {
            *x += DirToVDir[dir].x;
            if (*x >= GetMapMaster()->GetMapW() ||
                *x <= -1)
                *x -= DirToVDir[dir].x;
        }
        if (y)
        {
            *y += DirToVDir[dir].y;
            if (*y >= GetMapMaster()->GetMapH() ||
                *y <= -1)
                *y -= DirToVDir[dir].y;
        }
        if (z)
        {
            *z += DirToVDir[dir].z;
            if (*z >= GetMapMaster()->GetMapD() ||
                *z <= -1)
                *z -= DirToVDir[dir].z;
        }
    }
    inline int dir_to_byond(Dir dir)
    {
        return DIR_TO_BYOND[dir];
    }
}