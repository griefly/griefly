#include "Turf.h"
#include "MapClass.h"
#include "Liquid.h"

#include "sync_random.h"

//itemAdder<CGround> adder;
//itemAdder<Pit> adder2;

CGround::CGround()
{ 
    SetSprite("icons/space.png");
    
    std::stringstream conv;
    size_t value = get_rand() % 25 + 1;
    conv << value;
    SetState(conv.str());

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