#include "Ghost.h"

#include "MapClass.h"

Ghost::Ghost()
{
    v_level = 11;

    SetSprite("icons/mob.dmi");
    SetState("ghost_grey");

    passable_level = Passable::EMPTY;

    name = "Ghost";
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