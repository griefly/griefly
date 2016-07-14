#pragma once

#include ""

class Tables: public IMovable
{
public:
    DECLARE_SAVED(Tables, IMovable);
    DECLARE_GET_TYPE_ITEM(Tables);
    Tables(size_t id);
    virtual void AttackBy(id_ptr_on<Item> item) override;
private:
    bool up = 0;
    bool down = 0;
    bool left  = 0;
    bool right = 0;
};
ADD_TO_TYPELIST(Tables);
