#include "Floor.h"

Floor::Floor()
{
    transparent = true;
    passable = true;

    SetSprite("icons/floors.dmi");
    SetState("floor");

    name = "Floor";
}