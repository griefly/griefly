#include "Computers.h"

using namespace kv;

Computer::Computer()
{
    SetTransparency(true);
    SetPassable(Dir::ALL, passable::AIR);

    SetSprite("icons/computer.dmi");
    SetState("securityb");

    v_level = 3;

    SetName("Computer");
}
