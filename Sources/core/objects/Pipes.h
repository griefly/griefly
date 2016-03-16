#pragma once

#include "Movable.h"

class Pipe: public IMovable
{
public:
    DECLARE_SAVED(Pipe, IMovable);
    DECLARE_GET_TYPE_ITEM(Pipe);
    Pipe(size_t id);
};
ADD_TO_TYPELIST(Pipe);
