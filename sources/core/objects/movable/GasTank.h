#pragma once

#include "Movable.h"

#include "atmos/AtmosHolder.h"
#include "objects/movable/items/Item.h"

namespace kv
{

class GasTank : public Movable
{
public:
    DECLARE_SAVEABLE(GasTank, Movable);
    REGISTER_CLASS_AS(GasTank);
    GasTank();
    virtual void AfterWorldCreation() override;

    virtual void AttackBy(IdPtr<Item> item) override;
    virtual void Process() override;

    atmos::AtmosHolder* GetAtmosHolder() { return &atmos_holder_; }
private:
    void Open();
    void Close();

    bool KV_SAVEABLE(open_);
    atmos::AtmosHolder KV_SAVEABLE(atmos_holder_);
};
END_DECLARE(GasTank);

class MagicGasTank : public GasTank
{
public:
    DECLARE_SAVEABLE(MagicGasTank, GasTank);
    REGISTER_CLASS_AS(MagicGasTank);
    MagicGasTank();
    virtual void AfterWorldCreation() override;
    virtual void Process() override;
};
END_DECLARE(MagicGasTank);

class PlasmaGasTank : public GasTank
{
public:
    DECLARE_SAVEABLE(PlasmaGasTank, GasTank);
    REGISTER_CLASS_AS(PlasmaGasTank);
    PlasmaGasTank();
};
END_DECLARE(PlasmaGasTank);

}
