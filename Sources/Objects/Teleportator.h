#pragma once

#include <queue>

#include "MessageReceiver.h"

class Teleportator : public IMessageReceiver
{
public:
    DECLARE_SAVED(Teleportator, IMessageReceiver);
    DECLARE_GET_TYPE_ITEM(Teleportator);
    Teleportator(size_t id);
    virtual void processGUImsg(const Message& msg) override;
protected:
    void CreateItem(unsigned int from_id, const std::string& hash);
};

ADD_TO_TYPELIST(Teleportator);
