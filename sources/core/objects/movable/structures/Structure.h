#pragma once

#include "objects/movable/Movable.h"

namespace kv
{

class Structure : public Movable
{
public:
    DECLARE_SAVEABLE(Structure, Movable);
    REGISTER_CLASS_AS(Structure);
    Structure() { SetVisibleLevel(visible_level::STRUCTURE); }
    virtual void AttackBy(IdPtr<Item> item) override;
};
END_DECLARE(Structure);

}
