#pragma once

#include "Turf.h"

namespace kv
{

class MetalWall : public ITurf
{
public:
    DECLARE_SAVEABLE(MetalWall, ITurf);
    REGISTER_CLASS_AS(MetalWall);
    MetalWall();
    virtual void AttackBy(IdPtr<Item> item) override;
};
END_DECLARE(MetalWall);

class ReinforcedWall : public ITurf
{
public:
    DECLARE_SAVEABLE(ReinforcedWall, ITurf);
    REGISTER_CLASS_AS(ReinforcedWall);
    ReinforcedWall();
};
END_DECLARE(ReinforcedWall);

}
