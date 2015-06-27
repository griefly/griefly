#pragma once

#include "Item.h"

class SimpleCloth: public Item
{
public:
    DECLARE_SAVED(SimpleCloth, Item);
    DECLARE_GET_TYPE_ITEM(SimpleCloth);
    SimpleCloth(size_t id);
};
ADD_TO_TYPELIST(SimpleCloth);



