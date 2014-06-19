#pragma once

class Manager;
class MapEditor;

namespace mob_position 
{
    Manager* mng();
    void set_mng(Manager* mng);
    int get_shift_x();
    int get_shift_y();

    void set_map_editor(MapEditor* map_editor);
    MapEditor* map_editor();
}