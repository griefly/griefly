#pragma once

#include "Movable.h"
#include "Network2.h"

class IMessageReceiver : public IMovable
{
public:
    DECLARE_SAVED(IMessageReceiver, IMovable);
    DECLARE_GET_TYPE_ITEM(IMessageReceiver);
    virtual void processGUImsg(const Message2& msg){};
    IMessageReceiver(size_t id) : IMovable(id) {}
};

ADD_TO_TYPELIST(IMessageReceiver);
