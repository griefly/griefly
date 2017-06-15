#pragma once

#include "Movable.h"

#include "core/objects/Tile.h"
#include "core/objects/movable/items/Item.h"

namespace kv
{

class Closet : public Movable
{
public:
    DECLARE_SAVEABLE(Closet, Movable);
    REGISTER_CLASS_AS(Closet);
    Closet();

    virtual void AttackBy(IdPtr<Item> item) override;
    virtual void Bump(IdPtr<Movable> item) override;

    virtual bool Contains(IdPtr<MapObject> item) const override;
    virtual bool CanTouch(IdPtr<MapObject> item) const override;

    virtual bool AddObject(IdPtr<MapObject> item) override;
    virtual bool RemoveObject(IdPtr<MapObject> item) override;

    virtual void AfterWorldCreation() override;

    virtual void Delete() override;
    virtual IdPtr<MapObject> GetNeighbour(Dir direct) const override;
    // TODO: ForEach (?)
protected:
    virtual void Open();
    virtual void Close();

    bool KV_SAVEABLE(open_);

    std::vector<IdPtr<Movable>> KV_SAVEABLE(content_);
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
