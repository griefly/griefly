#include "Turf.h"
#include "MapClass.h"
#include "Liquid.h"

//itemAdder<CGround> adder;
//itemAdder<Pit> adder2;

CGround::CGround()
{ 
    SetSprite("icons/turf.png");
    passable = false;
    transparent = false;
    v_level = 1;
    name = "Turf";
};

Pit::Pit()
{
    SetSprite("icons/pit.png");
    passable = true;
    transparent = true;
    name = "Pit";
}