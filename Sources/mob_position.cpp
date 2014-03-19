#include "mob_position.h"

#include "Mob.h"
#include "constheader.h"

#include "constheader.h"

namespace mob_position 
{
    Manager* mng_;
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
        return mng_->thisMob->GetDrawX() * (-1) + (sizeW / 2); // TODO
    }
    int get_shift_y()
    {
        return mng_->thisMob->GetDrawY() * (-1) + (sizeH / 2) - 16;
    }
}