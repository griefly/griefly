#pragma once

#include "Item.h"

class Rod: public Item
{
public:
    DECLARE_SAVED(Rod, Item);
    DECLARE_GET_TYPE_ITEM(Rod);
    Rod();
};
ADD_TO_TYPELIST(Rod);