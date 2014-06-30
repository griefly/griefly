#pragma once

#include "OnMapInt.h"

class ITurf : public IOnMapItem
{
public:
    DECLARE_SAVED(ITurf, IOnMapItem);
    DECLARE_GET_TYPE_ITEM(ITurf);
    ITurf() {};
    virtual void delThis() override; 
};
ADD_TO_TYPELIST(ITurf);