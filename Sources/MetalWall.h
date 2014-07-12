#pragma once

#include "ITurf.h"

class MetalWall: public ITurf
{
public:
    DECLARE_SAVED(MetalWall, ITurf);
    DECLARE_GET_TYPE_ITEM(MetalWall);
    MetalWall();
};
ADD_TO_TYPELIST(MetalWall);

class ReinforcedWall: public ITurf
{
public:
    DECLARE_SAVED(ReinforcedWall, ITurf);
    DECLARE_GET_TYPE_ITEM(ReinforcedWall);
    ReinforcedWall();
};
ADD_TO_TYPELIST(ReinforcedWall);