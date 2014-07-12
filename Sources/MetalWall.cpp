#include "MetalWall.h"

MetalWall::MetalWall()
{
    transparent = false;
    passable = false;

    SetSprite("icons/walls.dmi");
    SetState("metal0");

    name = "Metal wall";
}

ReinforcedWall::ReinforcedWall()
{
    transparent = false;
    passable = false;

    SetSprite("icons/walls.dmi");
    SetState("r_wall");

    name = "Reinforced wall";
}