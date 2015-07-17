#pragma once

#include "IMovable.h"

class Tank: public IMovable
{
public:
    DECLARE_SAVED(Tank, IMovable);
    DECLARE_GET_TYPE_ITEM(Tank1112);
    Tank(size_t id) : IMovable(id) { }
};
ADD_TO_TYPELIST(Tank);

class FuelTank: public Tank
{
public:
    DECLARE_SAVED(FuelTank, Tank);
    DECLARE_GET_TYPE_ITEM(FuelTank1113);
    FuelTank(size_t id);
    virtual void AttackBy(id_ptr_on<Item> item) override;
};
ADD_TO_TYPELIST(FuelTank);

class WaterTank: public Tank
{
public:
    DECLARE_SAVED(WaterTank, Tank);
    DECLARE_GET_TYPE_ITEM(WaterTank1114);
    WaterTank(size_t id);
    virtual void AttackBy(id_ptr_on<Item> item) override;
};
ADD_TO_TYPELIST(WaterTank);
