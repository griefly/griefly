#pragma once

#include "Turf.h"
#include <bitset>

namespace kv
{
class MetalWall : public Turf
{
public:
    DECLARE_SAVEABLE(MetalWall, Turf);
    REGISTER_CLASS_AS(MetalWall);
    MetalWall();

    virtual void AttackBy(IdPtr<Item> item) override;
    
    void UpdateState(Dir dir = Dir::ALL);
    void UpdateNeighborhoodState(Dir dir = Dir::ALL);
    void CheckNeighborhood(Dir dir = Dir::ALL);
    
    virtual void AfterWorldCreation() override;
    virtual void Delete() override;
private:
    QString default_state_;
    std::bitset<4> current_state_;
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
