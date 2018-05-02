#pragma once

#include "Item.h"

namespace kv
{

class Limb : public Item
{
public:
    DECLARE_SAVEABLE(Limb, Item);
    REGISTER_CLASS_AS(Limb);
    Limb();
};
END_DECLARE(Limb)

}
