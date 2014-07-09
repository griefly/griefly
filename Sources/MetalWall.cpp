#include "MetalWall.h"

MetalWall::MetalWall()
{
    transparent = false;
    passable = false;

    SetSprite("icons/walls.dmi");
    SetState("metal0");

    name = "Metal wall";
}