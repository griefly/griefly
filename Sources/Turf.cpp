#include "Turf.h"
#include "MapClass.h"
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

    SetPassable(D_ALL, Passable::EMPTY);
    transparent = false;
    v_level = 1;
    name = "Turf";
};

Pit::Pit()
{
    SetSprite("icons/pit.png");
    SetPassable(D_ALL, Passable::FULL);
    transparent = true;
    name = "Pit";
}