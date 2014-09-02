#include "Ghost.h"

#include "MapClass.h"
#include "Creator.h"

Ghost::Ghost()
{
    v_level = 11;

    SetSprite("icons/mob.dmi");
    SetState("ghost_grey");

    passable_level = Passable::EMPTY;

    name = "Ghost";
}

bool Ghost::IsMobGhost()
{
    static size_t mob_id = 0;
    static bool draw = true;
    if (mob_id != GetMob().ret_id())
    {
        if (id_ptr_on<Ghost> g = GetMob())
            draw = true;
        else
            draw = false;
        mob_id = GetMob().ret_id();
    }
    return draw;
}

void Ghost::processImage(DrawType type)
{
    if (IsMobGhost())
        IMob::processImage(type);
}

bool Ghost::IsTransp(int x, int y)
{
    if (IsMobGhost())
        return IMob::IsTransp(x, y);
    return true;
}

void Ghost::CalculateVisible(std::list<point>* visible_list)
{
    visible_list->clear();
    point p;
    p.posz = GetZ();
    int x_low_border = std::max(0, GetX() - sizeHsq + 1);
    int x_high_border = std::min(GetMapMaster()->GetMapW(), GetX() + sizeHsq);
    int y_low_border = std::max(0, GetY() - sizeWsq + 1);
    int y_high_border = std::max(GetMapMaster()->GetMapH(), GetY() + sizeWsq);
    for (int i = x_low_border; i < x_high_border; ++i)
        for (int j = y_low_border; j < y_high_border; ++j)
        {
            p.posx = i;
            p.posy = j;
            visible_list->push_back(p);
        }
}