#pragma once

#include "Turf.h"
#include <bitset>
#include <array>
#include <iostream>
#include <sstream>

namespace kv
{

const std::array<Dir, 4> WALL_PROCESSING_DIRS = {Dir::NORTH, Dir::SOUTH, Dir::WEST, Dir::EAST};

class WallBitMask
{
friend inline FastDeserializer& operator>>(FastDeserializer& file, kv::WallBitMask& wbm);
friend inline FastSerializer& operator<<(FastSerializer& file, const kv::WallBitMask& wbm);
public:
    WallBitMask();

    void SetBitByDirection(const Dir dir, const bool value);
    QString GetState(const QString& default_state);

    operator qint32();
private:
    std::bitset<4> value_;
};

inline FastDeserializer& operator>>(FastDeserializer& file, kv::WallBitMask& wbm)
{
    qint32 number;
    file >> number;
    wbm.value_ = std::bitset<4>(number);
    return file;
}

inline FastSerializer& operator<<(FastSerializer& file, const kv::WallBitMask& wbm)
{
    file << static_cast<qint32>(wbm.value_.to_ulong());
    return file;
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
    WallBitMask KV_SAVEABLE(current_mask_);
    QString KV_SAVEABLE(default_state_);
private:
    void UpdateState(const Dir dir = Dir::ALL);
    void NotifyNeighborhood(const Dir dir = Dir::ALL);
    void CheckNeighborhood(const Dir dir = Dir::ALL);
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
