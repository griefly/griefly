#pragma once

#include "Movable.h"

#include "Tile.h"
#include "Item.h"

namespace kv
{

class Closet : public IMovable
{
public:
    DECLARE_SAVEABLE(Closet, IMovable);
    REGISTER_CLASS_AS(Closet);
    Closet();

    virtual void AttackBy(IdPtr<Item> item) override;
    virtual void Bump(IdPtr<IMovable> item) override;

    virtual bool Contains(IdPtr<IOnMapBase> item) const override;
    virtual bool CanTouch(IdPtr<IOnMapBase> item) const override;

    virtual bool AddItem(IdPtr<IOnMapBase> item) override;
    virtual bool RemoveItem(IdPtr<IOnMapBase> item) override;

    virtual void AfterWorldCreation() override;

    virtual void Delete() override;
    virtual IdPtr<IOnMapBase> GetNeighbour(Dir direct) const override;
    // TODO: ForEach (?)
protected:
    virtual void Open();
    virtual void Close();

    bool KV_SAVEABLE(open_);

    std::vector<IdPtr<IMovable>> KV_SAVEABLE(content_);
};
END_DECLARE(Closet);

class SecurityLocker : public Closet
{
public:
    DECLARE_SAVEABLE(SecurityLocker, Closet);
    REGISTER_CLASS_AS(SecurityLocker);
    SecurityLocker();
    virtual void AttackBy(IdPtr<Item> item) override;
protected:
    virtual void Lock();
    virtual void Unlock();

    virtual void Open() override;
    virtual void Close() override;
    bool KV_SAVEABLE(locked_);
};
END_DECLARE(SecurityLocker);

}
