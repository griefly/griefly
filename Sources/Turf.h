#pragma once

#include "OnMapInt.h"
#include "ITurf.h"

class CTurf: public ITurf
{
public:
    DECLARE_GET_TYPE_ITEM(cturf)
    DECLARE_SAVED(CTurf, ITurf);
    CTurf(){};
    
};
ADD_TO_TYPELIST(CTurf);

class CGround: public CTurf
{
public:
    DECLARE_GET_TYPE_ITEM(ground);
    DECLARE_SAVED(CGround, CTurf);
    CGround();
};
ADD_TO_TYPELIST(CGround);

class Pit: public CTurf
{
public:
    Pit();
    DECLARE_SAVED(Pit, CTurf);
    DECLARE_GET_TYPE_ITEM(pit)
};
ADD_TO_TYPELIST(Pit);