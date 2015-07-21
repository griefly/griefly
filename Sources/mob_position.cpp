#include "mob_position.h"

#include "Mob.h"
#include "constheader.h"
#include "TileInt.h"
#include "Creator.h"

namespace mob_position 
{
    int get_shift_x() 
    {
        if (GetManager())
            return GetMob()->GetDrawX() * (-1) + (sizeW / 2); // TODO
        return 0;
    }
    int get_shift_y()
    {
        if (GetManager())
            return GetMob()->GetDrawY() * (-1) + (sizeH / 2) - 16;
        return 0;
    }
    int get_mob_z()
    {
        if (GetManager())
            return GetMob()->GetZ();
        return 0;
    }

}
