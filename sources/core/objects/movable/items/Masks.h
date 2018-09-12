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

class GasMask : public Mask
{
public:
    DECLARE_SAVEABLE(GasMask, Mask);
    REGISTER_CLASS_AS(GasMask);
    GasMask();
};
END_DECLARE(GasMask);

}

