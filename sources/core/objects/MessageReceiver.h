#pragma once

#include "Movable.h"
#include "net/Network2.h"

class IMessageReceiver : public kv::IMovable
{
public:
    DECLARE_SAVED(IMessageReceiver, IMovable);
    DECLARE_GET_TYPE_ITEM(IMessageReceiver);
    virtual void ProcessMessage(const Message2& msg) { }
    IMessageReceiver() { }
};

ADD_TO_TYPELIST(IMessageReceiver);
