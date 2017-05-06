#pragma once

#include "MainObject.h"

class IDraw : public kv::Object
{
public:
    DECLARE_SAVED(IDraw, Object);
    DECLARE_GET_TYPE_ITEM(IDraw)
    IDraw();
    virtual void Represent() {}
};

ADD_TO_TYPELIST(IDraw);
