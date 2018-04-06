#include "Computers.h"

using namespace kv;

Computer::Computer()
{
    SetTransparency(true);
    SetPassable(Dir::ALL, passable::AIR);

    SetSprite("icons/computer.dmi");
    SetState("securityb");

    SetVisibleLevel(3);

    SetName("Computer");
}
