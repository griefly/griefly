#pragma once

#include "IMovable.h"

class Item: public IMovable
{
public:
    DECLARE_SAVED(Item, IMovable);
    DECLARE_GET_TYPE_ITEM(Item);
    Item() {};
};
ADD_TO_TYPELIST(Item);