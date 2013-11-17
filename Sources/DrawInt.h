#pragma once

#include <Typelist.h>

#include "MainInt.h"
#include "NetClientImpl.h"

class IDraw: public IMainItem
{
public:
    // TODO: это - интерфейс. Сместить кое-какую реализацию вниз
    DECLARE_GET_TYPE_ITEM(IDraw)
    DECLARE_SAVED(IDraw, IMainItem);
    IDraw();
    virtual void processImage(SDL_Surface* surface);//draw this item
    virtual bool IsTransp(int x, int y) { return true;};
    virtual int GetDrawX() const { return 0; }
    virtual int GetDrawY() const { return 0; }
};

ADD_TO_TYPELIST(IDraw);