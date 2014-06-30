#pragma once

#include "OnMapInt.h"

class IMovable : public IOnMapItem
{
public:
    DECLARE_GET_TYPE_ITEM(IMovable);
    DECLARE_SAVED(IMovable, IOnMapItem);
    IMovable();
    void processMove();//for move
    void move(Dir direct);//pix move
    virtual bool checkMove(Dir direct);
    bool checkMoveTime();
    bool checkPassable();
    bool mainMove();

    virtual void delThis() override { IOnMapItem::delThis(); }
public:
    int KV_SAVEBLE(lastMove);
    int KV_SAVEBLE(tickSpeed);
    int KV_SAVEBLE(pixSpeed);
    Dir KV_SAVEBLE(dMove);
};
ADD_TO_TYPELIST(IMovable);