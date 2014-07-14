#include "MetalWall.h"

MetalWall::MetalWall()
{
    transparent = false;
    SetPassable(D_ALL, false);

    SetSprite("icons/walls.dmi");
    SetState("metal0");

    name = "Metal wall";
}

ReinforcedWall::ReinforcedWall()
{
    transparent = false;
    SetPassable(D_ALL, false);

    SetSprite("icons/walls.dmi");
    SetState("r_wall");

    name = "Reinforced wall";
}