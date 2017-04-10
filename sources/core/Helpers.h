#pragma once

#include <cassert>

#include "Constheader.h"
#include "Map.h"

#include "representation/Screen.h"

namespace helpers
{
    const int DIR_TO_BYOND[10] = {3, 2, 1, 0, 3, 3, 4, 5, 6, 7};

    inline int DirToByond(Dir dir)
    {
        // TODO: switch
        return DIR_TO_BYOND[static_cast<int>(dir)];
    }
}
