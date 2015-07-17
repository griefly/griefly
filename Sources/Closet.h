#pragma once

#include "IMovable.h"

#include "TileInt.h"
#include "Item.h"

class Closet: public IMovable
{
public:
    DECLARE_SAVED(Closet, IMovable);
    DECLARE_GET_TYPE_ITEM(Closet);
    Closet(size_t id);

    virtual void AttackBy(id_ptr_on<Item> item) override;
    virtual void Bump(id_ptr_on<IMovable> item) override;

    virtual bool Contains(id_ptr_on<IOnMapBase> item) const override;

    virtual bool AddItem(id_ptr_on<IOnMapBase> item) override;
    virtual bool RemoveItem(id_ptr_on<IOnMapBase> item) override;

    // TODO: ForEach (?)
private:
    void Open();
    void Close();

    bool KV_SAVEBLE(open_);

    std::vector<id_ptr_on<IMovable>> KV_SAVEBLE(content_);
};
ADD_TO_TYPELIST(Closet);
