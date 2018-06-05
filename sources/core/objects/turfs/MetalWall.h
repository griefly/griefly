#pragma once

#include "Turf.h"
#include <bitset>
#include <array>

namespace kv
{

const std::array<Dir, 4> WALL_PROCESSING_DIRS = {Dir::NORTH, Dir::SOUTH, Dir::WEST, Dir::EAST};

class MetalWall : public Turf
{
public:
    DECLARE_SAVEABLE(MetalWall, Turf);
    REGISTER_CLASS_AS(MetalWall);

    MetalWall();

    void UpdateState(Dir dir = Dir::ALL);
    void UpdateNeighborhoodState(Dir dir = Dir::ALL);
    void CheckNeighborhood(Dir dir = Dir::ALL);

    virtual void AttackBy(IdPtr<Item> item) override;

    virtual void AfterWorldCreation() override;
    virtual void Delete() override;
protected:
    QString default_state_;
    std::bitset<4> current_state_;
};
END_DECLARE(MetalWall);

class ReinforcedWall : public MetalWall
{
public:
    DECLARE_SAVEABLE(ReinforcedWall, MetalWall);
    REGISTER_CLASS_AS(ReinforcedWall);
    ReinforcedWall();

    virtual void AttackBy(IdPtr<Item> item) override;

    virtual void AfterWorldCreation() override;
    virtual void Delete() override;
};
END_DECLARE(ReinforcedWall);

}
