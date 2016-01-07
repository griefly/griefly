#pragma once

#include "Movable.h"

class Structure: public IMovable
{
public:
    DECLARE_SAVED(Structure, IMovable);
    DECLARE_GET_TYPE_ITEM(Structure);
    Structure(size_t id) : IMovable(id) { v_level = 3; };
    virtual void AttackBy(id_ptr_on<Item> item) override;
};
ADD_TO_TYPELIST(Structure);
