#pragma once

#include <assert.h>

#include "Constheader.h"
#include "Map.h"

#include "representation/Screen.h"

namespace helpers
{
    const int DIR_TO_BYOND[10] = {3, 2, 1, 0, 3, 3, 4, 5, 6, 7};

    inline int dir_to_byond(Dir dir)
    {
        return DIR_TO_BYOND[dir];
    }
    inline void fix_borders(int* x, int* y, int* z)
    {
        if (x)
        {
            if (*x >= GetMap().GetMapW())
                *x = GetMap().GetMapW() - 1;
            if (*x < 0)
                *x = 0;
        }
        if (y)
        {
            if (*y >= GetMap().GetMapH())
                *y = GetMap().GetMapH() - 1;
            if (*y < 0)
                *y = 0;
        }
        if (z)
        {
            if (*z >= GetMap().GetMapD())
                *z = GetMap().GetMapD() - 1;
            if (*z < 0)
                *z = 0;
        }
    }
    inline bool check_borders(const int* x, const int* y, const int* z)
    {
        if (x)
        {
            if (*x >= GetMap().GetMapW())
                return false;
            if (*x < 0)
                return false;
        }
        if (y)
        {
            if (*y >= GetMap().GetMapH())
                return false;
            if (*y < 0)
                return false;
        }
        if (z)
        {
            if (*z >= GetMap().GetMapD())
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
               / static_cast<float>(GetScreen().w())
               ));
        *y = static_cast<int>
            (  static_cast<float>(*y) 
            * (  static_cast<float>(sizeH) 
               / static_cast<float>(GetScreen().h())
               ));
    }
}
