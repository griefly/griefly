#pragma once

#include "OnMapInt.h"
#include "AtmosHolder.h"

class ITurf : public IOnMapObject
{
public:
    DECLARE_SAVED(ITurf, IOnMapObject);
    DECLARE_GET_TYPE_ITEM(ITurf);
    ITurf(size_t id) : IOnMapObject(id) { atmos_state_ = SIMULATED; };
    virtual void delThis() override;
    AtmosState GetAtmosState() const { return atmos_state_; }
    void SetAtmosState(AtmosState as) { atmos_state_ = as; }
private:
    AtmosState KV_SAVEBLE(atmos_state_);
};
ADD_TO_TYPELIST(ITurf);
