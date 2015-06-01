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
    inline void fix_borders(int* x, int* y, int* z)
    {
        if (x)
        {
            if (*x >= GetMapMaster()->GetMapW())
                *x = GetMapMaster()->GetMapW() - 1;
            if (*x < 0)
                *x = 0;
        }
        if (y)
        {
            if (*y >= GetMapMaster()->GetMapH())
                *y = GetMapMaster()->GetMapH() - 1;
            if (*y < 0)
                *y = 0;
        }
        if (z)
        {
            if (*z >= GetMapMaster()->GetMapD())
                *z = GetMapMaster()->GetMapD() - 1;
            if (*z < 0)
                *z = 0;
        }
    }
    inline bool check_borders(int* x, int* y, int* z)
    {
        if (x)
        {
            if (*x >= GetMapMaster()->GetMapW())
                return false;
            if (*x < 0)
                return false;
        }
        if (y)
        {
            if (*y >= GetMapMaster()->GetMapH())
                return false;
            if (*y < 0)
                return false;
        }
        if (z)
        {
            if (*z >= GetMapMaster()->GetMapD())
                return false;
            if (*z < 0)
                return false;
        }
        return true;
    }
    inline Dir revert_dir(Dir direct)
    {
        return DirToRDir[direct];
    }
    inline void normalize_pixels(int* x, int* y)
    {
        *x = static_cast<int>
            (  static_cast<float>(*x) 
            * (  static_cast<float>(sizeW)
               / static_cast<float>(GetScreen()->w())
               ));
        *y = static_cast<int>
            (  static_cast<float>(*y) 
            * (  static_cast<float>(sizeH) 
               / static_cast<float>(GetScreen()->h())
               ));
    }
}
