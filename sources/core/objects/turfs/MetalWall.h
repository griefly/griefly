#pragma once

#include "Turf.h"
#include <bitset>
#include <array>

namespace kv
{

const std::array<Dir, 4> WALL_PROCESSING_DIRS = {Dir::NORTH, Dir::SOUTH, Dir::WEST, Dir::EAST};

inline FastDeserializer& operator>>(FastDeserializer& file, std::bitset<4>& bset)
{
    qint32 number;
    file >> number;
    bset = std::bitset<4>(number);
    return file;
}

inline FastSerializer& operator<<(FastSerializer& file, const std::bitset<4>& bset)
{
    file << static_cast<qint32>(bset.to_ulong());
    return file;
}

inline unsigned int Hash(const std::bitset<4>& value)
{
    return value.to_ulong();
}

class MetalWall : public Turf
{
public:
    DECLARE_SAVEABLE(MetalWall, Turf);
    REGISTER_CLASS_AS(MetalWall);

    MetalWall();

    virtual void AttackBy(IdPtr<Item> item) override;
    virtual void AfterWorldCreation() override;
    virtual void Delete() override;
protected:
    std::bitset<4> KV_SAVEABLE(current_state_);
    QString KV_SAVEABLE(default_state_);
private:
    void UpdateState(const Dir dir = Dir::ALL);
    void NotifyNeighborhood(const Dir dir = Dir::ALL);
    void CheckNeighborhood(const Dir dir = Dir::ALL);
    void SetBitByDirection(const Dir dir, const bool value);
};
END_DECLARE(MetalWall);

class ReinforcedWall : public MetalWall
{
public:
    DECLARE_SAVEABLE(ReinforcedWall, MetalWall);
    REGISTER_CLASS_AS(ReinforcedWall);
    ReinforcedWall();

    virtual void AttackBy(IdPtr<Item> item) override;
};
END_DECLARE(ReinforcedWall);

}
