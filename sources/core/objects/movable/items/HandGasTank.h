#pragma once

#include "Item.h"

namespace kv
{

class HandGasTank : public Item
{
public:
    DECLARE_SAVEABLE(HandGasTank, Item);
    REGISTER_CLASS_AS(HandGasTank);
    HandGasTank();
};
END_DECLARE(HandGasTank);

}

