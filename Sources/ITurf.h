#pragma once

#include "OnMapBaseInt.h"

class ITurf: public IOnMapBase
{
public:
    DECLARE_SAVED(ITurf, IOnMapBase);
    DECLARE_GET_TYPE_ITEM(IOnMapBase);
    ITurf() {};
    virtual bool IsPassable() const { return true; }
    virtual bool IsTransparent() const { return true; }
    virtual void delThis() override { IOnMapBase::delThis(); }
};
ADD_TO_TYPELIST(ITurf);