#pragma once

#include "ITurf.h"

class Floor: public ITurf
{
public:
    DECLARE_SAVED(Floor, ITurf);
    DECLARE_GET_TYPE_ITEM(Floor);
    Floor();
};
ADD_TO_TYPELIST(Floor);
