#pragma once

#include "OnMapInt.h"

class Door: public IOnMapItem
{
public:
    DECLARE_SAVED(Door, IOnMapItem);
    DECLARE_GET_TYPE_ITEM(Door);
    Door();
    
    virtual void Open();
    virtual void Close();
    virtual void process() override;
    bool IsOpen() const { return door_state_ == OPEN; }
private:
    enum STATE
    {
        OPEN, CLOSED, OPENING, CLOSING   
    };
    int KV_SAVEBLE(door_state_);
    size_t KV_SAVEBLE(last_tick_);
};
ADD_TO_TYPELIST(Door);