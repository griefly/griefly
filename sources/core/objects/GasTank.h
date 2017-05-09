#pragma once

#include "Movable.h"

#include "../atmos/AtmosHolder.h"
#include "Item.h"

class GasTank : public kv::IMovable
{
public:
    DECLARE_SAVED(GasTank, IMovable);
    DECLARE_GET_TYPE_ITEM(GasTank);
    GasTank();
    virtual void AfterWorldCreation() override;

    virtual void AttackBy(IdPtr<kv::Item> item) override;
    virtual void Process() override;

    atmos::AtmosHolder* GetAtmosHolder() { return &atmos_holder_; }
private:
    void Open();
    void Close();

    bool KV_SAVEBLE(open_);
    atmos::AtmosHolder KV_SAVEBLE(atmos_holder_);
};
ADD_TO_TYPELIST(GasTank);

class MagicGasTank : public GasTank
{
public:
    DECLARE_SAVED(MagicGasTank, GasTank);
    DECLARE_GET_TYPE_ITEM(MagicGasTank);
    MagicGasTank();
    virtual void AfterWorldCreation() override;
    virtual void Process() override;
};
ADD_TO_TYPELIST(MagicGasTank);

class PlasmaGasTank : public GasTank
{
public:
    DECLARE_SAVED(PlasmaGasTank, GasTank);
    DECLARE_GET_TYPE_ITEM(PlasmaGasTank);
    PlasmaGasTank();
};
ADD_TO_TYPELIST(PlasmaGasTank);
