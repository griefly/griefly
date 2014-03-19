#pragma once

#include <functional>

#include "DrawInt.h"


class IOnMapBase: public IDraw
{
public:
    DECLARE_SAVED(IOnMapBase, IDraw);
    DECLARE_GET_TYPE_ITEM(IOnMapBase);
    IOnMapBase() {owner = 0;};
    virtual bool IsVisibleByPlayer() const
    {
        return true;
    }
    virtual void ForEach(std::function<void(id_ptr_on<IOnMapBase>)> callback)
    {
        return;
    }
    virtual bool IsPassable() const
    {
        return true;
    }
    virtual void delThis() override
    {
        if (owner.valid())
            owner->RemoveItem(id);
        IDraw::delThis();
    }

    // Add some item
    // True - added
    // False - failed
    // This must not remove item from old place
    virtual bool AddItem(id_ptr_on<IOnMapBase> item)
    {
        if (owner.valid()) 
            return owner->AddItem(item);
        return false;
    }
    // Remove some item
    // True - removed
    // False failed
    virtual bool RemoveItem(id_ptr_on<IOnMapBase> item)
    {
        if (owner.valid())
            return owner->RemoveItem(item);
        return false;
    }
    // If id equal with object id, dir fail or something else (operation unsupported)
    virtual id_ptr_on<IOnMapBase> GetNeighbour(Dir direct)
    {
        if (owner.valid())
            return owner->GetNeighbour(direct);
        return id;
    }
    template<class T>
    id_ptr_on<T> GetItem()
    {
        return GetItemImpl(T::RT_ITEM_S());
    }
    void SetOwner(size_t value)
    {
        owner = value;
    }
    id_ptr_on<IOnMapBase> GetOwner() const
    {
        return owner;
    }
protected:
    id_ptr_on<IOnMapBase> KV_SAVEBLE(owner);
    virtual size_t GetItemImpl(unsigned int hash) { return 0; }
};
ADD_TO_TYPELIST(IOnMapBase);