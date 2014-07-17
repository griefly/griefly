#pragma once

#include "IMovable.h"

class Structure: public IMovable
{
public:
    DECLARE_SAVED(Structure, IMovable);
    DECLARE_GET_TYPE_ITEM(Structure);
    Structure() {};
};
ADD_TO_TYPELIST(Structure);