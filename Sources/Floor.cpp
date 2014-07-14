#include "Floor.h"

Floor::Floor()
{
    transparent = true;
    SetPassable(D_ALL, true);

    SetSprite("icons/floors.dmi");
    SetState("floor");

    name = "Floor";
}