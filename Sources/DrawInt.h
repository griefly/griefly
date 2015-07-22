#pragma once

#include <Typelist.h>

#include "MainInt.h"
#include "NetClientImpl.h"

enum DrawType {SAME, TOP};

class IDraw: public IMainObject
{
public:
    DECLARE_SAVED(IDraw, IMainObject);
    DECLARE_GET_TYPE_ITEM(IDraw)
    IDraw(size_t id);
    virtual void processImage(DrawType type) {};//draw this item
    virtual bool IsTransp(int x, int y) { return true;};
    virtual int GetDrawX() const { return 0; }
    virtual int GetDrawY() const { return 0; }
};

ADD_TO_TYPELIST(IDraw);
