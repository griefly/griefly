#pragma once

#include "ITurf.h"

class Space: public ITurf
{
public:
    DECLARE_SAVED(Space, ITurf);
    DECLARE_GET_TYPE_ITEM(Space);
    Space(size_t id);
    virtual void AttackBy(id_ptr_on<Item> item) override;
};
ADD_TO_TYPELIST(Space);
