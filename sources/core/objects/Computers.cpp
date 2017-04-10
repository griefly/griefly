#include "Computers.h"

Computer::Computer(quint32 id) : IOnMapObject(id)
{
    transparent = true;
    SetPassable(Dir::ALL, Passable::AIR);

    SetSprite("icons/computer.dmi");
    SetState("securityb");

    v_level = 3;

    name = "Computer";
}
