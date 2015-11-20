#pragma once

#include "MainObject.h"

enum DrawType {SAME, TOP};

class IDraw: public IMainObject
{
public:
    DECLARE_SAVED(IDraw, IMainObject);
    DECLARE_GET_TYPE_ITEM(IDraw)
    IDraw(size_t id);
    virtual void processImage(DrawType type) {}
    virtual bool IsTransp(int x, int y) { return true;}
    virtual int GetDrawX() const { return 0; }
    virtual int GetDrawY() const { return 0; }
};

ADD_TO_TYPELIST(IDraw);
