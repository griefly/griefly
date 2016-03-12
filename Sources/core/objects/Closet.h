#pragma once

#include "Movable.h"

#include "Tile.h"
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
    virtual bool CanTouch(id_ptr_on<IOnMapBase> item) const override;

    virtual bool AddItem(id_ptr_on<IOnMapBase> item) override;
    virtual bool RemoveItem(id_ptr_on<IOnMapBase> item) override;

    virtual void AfterWorldCreation() override;

    // TODO: ForEach (?)
protected:
    virtual void Open();
    virtual void Close();

    bool KV_SAVEBLE(open_);

    std::vector<id_ptr_on<IMovable>> KV_SAVEBLE(content_);
};
ADD_TO_TYPELIST(Closet);

class SecurityLocker: public Closet
{
public:
    DECLARE_SAVED(SecurityLocker, Closet);
    DECLARE_GET_TYPE_ITEM(SecurityLocker);
    SecurityLocker(size_t id);
    virtual void AttackBy(id_ptr_on<Item> item) override;
protected:
    virtual void Lock();
    virtual void Unlock();

    virtual void Open() override;
    virtual void Close() override;
    bool KV_SAVEBLE(locked_);
};
ADD_TO_TYPELIST(SecurityLocker);
