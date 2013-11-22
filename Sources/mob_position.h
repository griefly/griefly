#pragma once

class Manager;

namespace mob_position 
{
    Manager* mng();
    void set_mng(Manager* mng);
    int get_shift_x();
    int get_shift_y();
}