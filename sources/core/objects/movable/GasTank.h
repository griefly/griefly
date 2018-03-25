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

    virtual void ApplyFire(int intensity) override;

    virtual atmos::AtmosHolder* GetAtmosHolder() override { return &atmos_holder_; }
private:
    void Open();
    void Close();
    void Break();

    enum State
    {
        OPEN,
        CLOSED,
        BROKEN
    };

    qint32 KV_SAVEABLE(state_);
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
