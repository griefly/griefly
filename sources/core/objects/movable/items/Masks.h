#pragma once

#include "Item.h"

namespace kv
{

class Mask : public Item
{
public:
    DECLARE_SAVEABLE(Mask, Item);
    REGISTER_CLASS_AS(Mask);
    Mask();
};
END_DECLARE(Mask);

}

