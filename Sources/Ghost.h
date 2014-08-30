#pragma once

#include "MobInt.h"

class Ghost: public IMob
{
public:
    DECLARE_GET_TYPE_ITEM(Ghost);
    DECLARE_SAVED(Ghost, IMob);
    Ghost();
    virtual void CalculateVisible(std::list<point>* visible_list) override;
};
ADD_TO_TYPELIST(Ghost);