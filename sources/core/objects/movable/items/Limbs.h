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

class Hand : public Limb
{
public:
    DECLARE_SAVEABLE(Hand, Limb);
    REGISTER_CLASS_AS(Hand);
    Hand();
};
END_DECLARE(Hand)

}
