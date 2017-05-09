#pragma once

#include "Movable.h"

namespace kv
{

class Bed : public IMovable
{
public:
    DECLARE_SAVED(Bed, IMovable);
    DECLARE_GET_TYPE_ITEM(Bed);
    Bed();
};
ADD_TO_TYPELIST(Bed);

class Chair : public IMovable
{
public:
    DECLARE_SAVED(Chair, IMovable);
    DECLARE_GET_TYPE_ITEM(Chair);
    Chair();

    virtual void AfterWorldCreation() override;
};
ADD_TO_TYPELIST(Chair);

class Stool : public IMovable
{
public:
    DECLARE_SAVED(Stool, IMovable);
    DECLARE_GET_TYPE_ITEM(Stool);
    Stool();
};
ADD_TO_TYPELIST(Stool);

}
