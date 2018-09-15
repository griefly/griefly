#pragma once

#include "Item.h"

namespace kv
{

class HandGasTank : public Item
{
public:
    DECLARE_SAVEABLE(HandGasTank, Item);
    REGISTER_CLASS_AS(HandGasTank);
    HandGasTank();

    virtual void AttackBy(IdPtr<Item> item) override;

    virtual atmos::AtmosHolder* GetAtmosHolder() override { return &atmos_holder_; }
private:
    atmos::AtmosHolder KV_SAVEABLE(atmos_holder_);
};
END_DECLARE(HandGasTank);

}

