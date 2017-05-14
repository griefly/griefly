#pragma once

#include "Movable.h"

namespace kv
{

class Structure : public IMovable
{
public:
    DECLARE_SAVEABLE(Structure, IMovable);
    REGISTER_CLASS_AS(Structure);
    Structure() { v_level = 3; };
    virtual void AttackBy(IdPtr<Item> item) override;
};
END_DECLARE(Structure);

}
