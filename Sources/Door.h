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
    bool IsOpen() const { return open_; }
private:
    bool KV_SAVEBLE(open_);
};
ADD_TO_TYPELIST(Door);