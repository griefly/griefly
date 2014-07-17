#pragma once

#include "OnMapInt.h"

class IMovable : public IOnMapObject
{
public:
    DECLARE_GET_TYPE_ITEM(IMovable);
    DECLARE_SAVED(IMovable, IOnMapObject);
    IMovable();
    void processMove();//for move
    void move(Dir direct);//pix move
    virtual bool checkMove(Dir direct);
    bool checkMoveTime();
    bool checkPassable();
    bool mainMove();

    virtual bool IsTransp(int x, int y) override;

    virtual void delThis() override { IOnMapObject::delThis(); }

    virtual void processImage(DrawType type);
    virtual void Bump(id_ptr_on<IMovable> item) override;
public:
    int KV_SAVEBLE(lastMove);
    int KV_SAVEBLE(tickSpeed);
    int KV_SAVEBLE(pixSpeed);
    Dir KV_SAVEBLE(dMove);
};
ADD_TO_TYPELIST(IMovable);