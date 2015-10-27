#include "mob_position.h"

#include "Manager.h"
#include "constheader.h"
#include "Tile.h"
#include "Creator.h"

namespace mob_position 
{
    int get_shift_x() 
    {
        if (GetMob().valid())
            return GetMob()->GetDrawX() * (-1) + (sizeW / 2); // TODO
        return 0;
    }
    int get_shift_y()
    {
        if (GetMob().valid())
            return GetMob()->GetDrawY() * (-1) + (sizeH / 2) - 16;
        return 0;
    }
    int get_mob_z()
    {
        if (GetMob().valid())
            return GetMob()->GetZ();
        return 0;
    }

}
