#pragma once

#include "Item.h"

namespace kv
{

class HandGasTank;

class Mask : public Item
{
public:
    DECLARE_SAVEABLE(Mask, Item);
    REGISTER_CLASS_AS(Mask);
    Mask();
};
END_DECLARE(Mask);

class GasMask : public Mask
{
public:
    DECLARE_SAVEABLE(GasMask, Mask);
    REGISTER_CLASS_AS(GasMask);
    GasMask();

    void SetGasTank(IdPtr<HandGasTank> gas_tank) { gas_tank_ = gas_tank; }
    IdPtr<HandGasTank> GetGasTank() const { return gas_tank_; }
private:
    IdPtr<HandGasTank> KV_SAVEABLE(gas_tank_);
};
END_DECLARE(GasMask);

}

