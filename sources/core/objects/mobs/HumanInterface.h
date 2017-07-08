#pragma once

#include "core/objects/Object.h"

namespace kv
{

class HumanInterface2 : public Object
{
public:
    DECLARE_SAVEABLE(HumanInterface2, Object);
    REGISTER_CLASS_AS(HumanInterface2);
    HumanInterface2();
};
END_DECLARE(HumanInterface2)

}

