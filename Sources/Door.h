#pragma once

#include "OnMapInt.h"

class Door: public IOnMapObject
{
public:
    DECLARE_SAVED(Door, IOnMapObject);
    DECLARE_GET_TYPE_ITEM(Door);
    Door();
    
    virtual void Open();
    virtual void Close();
    virtual void process() override;
    virtual void Bump(id_ptr_on<IMovable> item) override;
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