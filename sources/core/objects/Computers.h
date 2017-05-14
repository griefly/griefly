#pragma once

#include "OnMapObject.h"

namespace kv
{

class Computer : public IOnMapObject
{
public:
    DECLARE_SAVEABLE(Computer, IOnMapObject);
    REGISTER_CLASS_AS(Computer);
    Computer();
};
END_DECLARE(Computer);

}
