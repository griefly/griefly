#pragma once

#include "Turf.h"
#include <bitset>

namespace kv
{
class MetalWallInterface : public Turf
{
public:
    DECLARE_SAVEABLE(MetalWallInterface, Turf);
    REGISTER_CLASS_AS(MetalWallInterface);
    
    MetalWallInterface();
    
    void UpdateState(Dir dir = Dir::ALL);
    void UpdateNeighborhoodState(Dir dir = Dir::ALL);
    void CheckNeighborhood(Dir dir = Dir::ALL);
protected:
    QString default_state_;
    std::bitset<4> current_state_;
};
END_DECLARE(MetalWallInterface);

class MetalWall : public MetalWallInterface
{
public:
    DECLARE_SAVEABLE(MetalWall, MetalWallInterface);
    REGISTER_CLASS_AS(MetalWall);
    MetalWall();

    virtual void AttackBy(IdPtr<Item> item) override;

    virtual void AfterWorldCreation() override;
    virtual void Delete() override;
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
