#pragma once

#include "Movable.h"

namespace kv
{

class Tank : public IMovable
{
public:
    DECLARE_SAVEABLE(Tank, IMovable);
    REGISTER_CLASS_AS(Tank);
    Tank() { }
};
END_DECLARE(Tank);

class FuelTank : public Tank
{
public:
    DECLARE_SAVEABLE(FuelTank, Tank);
    REGISTER_CLASS_AS(FuelTank);
    FuelTank();
    virtual void AttackBy(IdPtr<Item> item) override;
};
END_DECLARE(FuelTank);

class WaterTank : public Tank
{
public:
    DECLARE_SAVEABLE(WaterTank, Tank);
    REGISTER_CLASS_AS(WaterTank);
    WaterTank();
    virtual void AttackBy(IdPtr<Item> item) override;
};
END_DECLARE(WaterTank);

}
