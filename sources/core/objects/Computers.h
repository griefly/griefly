#pragma once

#include "OnMapObject.h"

class Computer : public kv::IOnMapObject
{
public:
    DECLARE_SAVED(Computer, IOnMapObject);
    DECLARE_GET_TYPE_ITEM(Computer);
    Computer();
};
ADD_TO_TYPELIST(Computer);
