#pragma once

#include "ITurf.h"

class Floor: public ITurf
{
public:
    DECLARE_SAVED(Floor, ITurf);
    DECLARE_GET_TYPE_ITEM(Floor);
    Floor();
    virtual void AttackBy(id_ptr_on<Item> item) override;
    void SetOpen(bool o);
private:
    bool KV_SAVEBLE(open_);
};
ADD_TO_TYPELIST(Floor);
