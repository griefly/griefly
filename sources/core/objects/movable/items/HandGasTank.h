#pragma once

#include "Item.h"

namespace kv
{

class GasMask;

class HandGasTank : public Item
{
public:
    DECLARE_SAVEABLE(HandGasTank, Item);
    REGISTER_CLASS_AS(HandGasTank);
    HandGasTank();

    virtual void AttackBy(IdPtr<Item> item) override;

    void SetGasMask(IdPtr<GasMask> mask) { gas_mask_ = mask; }
    IdPtr<GasMask> GetGasMask() const { return gas_mask_; }

    virtual atmos::AtmosHolder* GetAtmosHolder() override { return &atmos_holder_; }
private:
    atmos::AtmosHolder KV_SAVEABLE(atmos_holder_);
    IdPtr<GasMask> KV_SAVEABLE(gas_mask_);
};
END_DECLARE(HandGasTank);

}

