#pragma once

#include "ITurf.h"

class Space: public ITurf
{
public:
    DECLARE_SAVED(Space, ITurf);
    DECLARE_GET_TYPE_ITEM(Space);
    Space();
};
ADD_TO_TYPELIST(Space);