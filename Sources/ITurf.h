#pragma once

#include "OnMapInt.h"

class ITurf : public IOnMapObject
{
public:
    DECLARE_SAVED(ITurf, IOnMapObject);
    DECLARE_GET_TYPE_ITEM(ITurf);
    ITurf() {};
    virtual void delThis() override; 
};
ADD_TO_TYPELIST(ITurf);