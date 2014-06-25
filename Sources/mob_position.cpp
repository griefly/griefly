#include "mob_position.h"

#include "MapEditor.h"
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
        return GetMapEditor()->GetViewerX() * 32 * (-1) + (sizeW / 2);
    }
    int get_shift_y()
    {
        if (GetManager())
            return GetMob()->GetDrawY() * (-1) + (sizeH / 2) - 16;
        return GetMapEditor()->GetViewerY() * 32 * (-1) + (sizeH / 2) - 16;
    }
    int get_mob_z()
    {
        if (GetManager())
            return castTo<CubeTile>(GetMob()->GetOwner().ret_item())->posz();
        return GetMapEditor()->GetViewerZ();
    }

}