#pragma once

#include "MainObject.h"

class IDraw : public kv::IMainObject
{
public:
    DECLARE_SAVED(IDraw, IMainObject);
    DECLARE_GET_TYPE_ITEM(IDraw)
    IDraw();
    virtual void Represent() {}
};

ADD_TO_TYPELIST(IDraw);
