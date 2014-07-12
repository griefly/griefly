#include "Floor.h"

Floor::Floor()
{
    transparent = true;
    passable = true;

    SetSprite("icons/floors.dmi");
    SetState("light_on_flicker1");

    name = "Floor";
}