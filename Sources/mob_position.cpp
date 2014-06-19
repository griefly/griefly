#include "mob_position.h"

#include "MapEditor.h"
#include "Mob.h"
#include "constheader.h"

namespace mob_position 
{
    MapEditor* map_editor_;
    MapEditor* map_editor()
    {
        return map_editor_;
    }
    void set_map_editor(MapEditor* map_e)
    {
        map_editor_ = map_e;
    }

    Manager* mng_ = nullptr;
    Manager* mng() 
    {
        return mng_;
    }
    void set_mng(Manager* mng)
    {
        mng_ = mng;
    }
    int get_shift_x() 
    {
        if (mng_)
            return mng_->thisMob->GetDrawX() * (-1) + (sizeW / 2); // TODO
        return map_editor_->GetViewerX() * 32 * (-1) + (sizeW / 2);
    }
    int get_shift_y()
    {
        if (mng_)
            return mng_->thisMob->GetDrawY() * (-1) + (sizeH / 2) - 16;
        return map_editor_->GetViewerY() * 32 * (-1) + (sizeH / 2) - 16;
    }
}