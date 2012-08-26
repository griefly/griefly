#pragma once

#include <queue>

#include "MessageReceiver.h"
#include "SyncQueue.h"

class Teleportator : public IMessageReceiver
{
public:
    DECLARE_SAVED(Teleportator, IMessageReceiver);
    DECLARE_GET_TYPE_ITEM(Teleportator);
    Teleportator();
    virtual void processGUImsg(std::string& msg) override;
    size_t WaitCreateItem();
protected:
    void CreateItem(size_t hash);
private:
    SynchronizedQueue<size_t> wait_queue_;
};

ADD_TO_TYPELIST(Teleportator);