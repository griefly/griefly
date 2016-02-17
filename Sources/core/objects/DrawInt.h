#pragma once

#include "MainObject.h"

class IDraw: public IMainObject
{
public:
    DECLARE_SAVED(IDraw, IMainObject);
    DECLARE_GET_TYPE_ITEM(IDraw)
    IDraw(size_t id);
    virtual void Represent() {}
};

ADD_TO_TYPELIST(IDraw);
