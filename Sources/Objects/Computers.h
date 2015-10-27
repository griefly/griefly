#pragma once

#include "OnMapObject.h"

class Computer: public IOnMapObject
{
public:
    DECLARE_SAVED(Computer, IOnMapObject);
    DECLARE_GET_TYPE_ITEM(Computer);
    Computer(size_t id);
};
ADD_TO_TYPELIST(Computer);
