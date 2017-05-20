#pragma once

#include "MaterialObject.h"

namespace kv
{

class Computer : public MaterialObject
{
public:
    DECLARE_SAVEABLE(Computer, MaterialObject);
    REGISTER_CLASS_AS(Computer);
    Computer();
};
END_DECLARE(Computer);

}
