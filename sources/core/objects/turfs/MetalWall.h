#pragma once

#include "Turf.h"

namespace kv
{

class MetalWall : public Turf
{
public:
    DECLARE_SAVEABLE(MetalWall, Turf);
    REGISTER_CLASS_AS(MetalWall);
    MetalWall();
    virtual void AttackBy(IdPtr<Item> item) override;
};
END_DECLARE(MetalWall);

class ReinforcedWall : public Turf
{
public:
    DECLARE_SAVEABLE(ReinforcedWall, Turf);
    REGISTER_CLASS_AS(ReinforcedWall);
    ReinforcedWall();
};
END_DECLARE(ReinforcedWall);

}
