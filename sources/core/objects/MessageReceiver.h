#pragma once

#include "Movable.h"
#include "net/Network2.h"

namespace kv
{

class IMessageReceiver : public IMovable
{
public:
    DECLARE_SAVEABLE(IMessageReceiver, IMovable);
    REGISTER_CLASS_AS(IMessageReceiver);
    virtual void ProcessMessage(const Message2& msg) { }
    IMessageReceiver() { }
};
END_DECLARE(IMessageReceiver);

}
