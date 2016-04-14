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
