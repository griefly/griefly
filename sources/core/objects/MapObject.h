#pragma once

#include <functional>

#include "Object.h"

namespace kv
{
class Turf;
class Movable;
class Item;

using PassableLevel = int;
namespace passable
{
    const PassableLevel EMPTY = 0;
    const PassableLevel AIR = 10;
    const PassableLevel SMALL_ITEM = 20;
    const PassableLevel BIG_ITEM = 30;
    const PassableLevel SMALL_CREATURE = 40;
    const PassableLevel BIG_CREATURE = 50;
    const PassableLevel FULL = 100;
}

inline bool CanPass(PassableLevel block, PassableLevel object)
{
    return block >= object;
}

class MapObject : public Object
{
public:
    DECLARE_SAVEABLE(MapObject, Object);
    REGISTER_CLASS_AS(MapObject);
    MapObject() { owner_ = 0; }
    virtual void ForEach(std::function<void(IdPtr<MapObject>)> callback)
    {
        return;
    }
    // 0 - passable
    // 10 - air passable
    //
    //
    virtual PassableLevel GetPassable(Dir direct) const
    {
        return passable::EMPTY;
    }
    virtual bool IsTransparent() const
    {
        return true;
    }
    
    virtual void Delete() override
    {
        if (owner_.IsValid())
        {
            owner_->RemoveObject(GetId());
        }
        Object::Delete();
    }

    virtual void AttackBy(IdPtr<Item> item);

    virtual void ApplyForce(VDir force) {}

    virtual bool CanTouch(IdPtr<MapObject> object) const
    {
        if (owner_.IsValid())
        {
            return owner_->CanTouch(object);
        }
        return false;
    }

    // Add some item
    // True - added
    // False - failed
    // This must not remove item from old place
    virtual bool AddObject(IdPtr<MapObject> object)
    {
        if (owner_.IsValid())
        {
            return owner_->AddObject(object);
        }
        return false;
    }
    // Remove some item
    // True - removed
    // False failed
    virtual bool RemoveObject(IdPtr<MapObject> object)
    {
        return false;
    }
    // If id equal with object id, dir fail or something else (operation unsupported)
    virtual IdPtr<MapObject> GetNeighbour(Dir direct) const
    {
        if (owner_.IsValid())
        {
            return owner_->GetNeighbour(direct);
        }
        return GetId();
    }

    virtual bool Contains(IdPtr<MapObject> object) const
    {
        Q_UNUSED(object);
        return false;
    }

    virtual void Bump(IdPtr<Movable> item);
    virtual void BumpByGas(Dir dir, bool inside = false);

    template<class T>
    IdPtr<T> GetItem()
    {
        return GetItemImpl(T::GetTypeIndexStatic());
    }
    void SetOwner(IdPtr<MapObject> owner)
    {
        owner_ = owner;
    }
    IdPtr<MapObject> GetOwner() const
    {
        return owner_;
    }

    virtual int GetX() const
    {
        return owner_->GetX();
    }
    virtual int GetY() const
    {
        return owner_->GetY();
    }
    virtual int GetZ() const
    {
        return owner_->GetZ();
    }
    virtual void Represent() { }
    virtual void UpdatePassable() { }
    virtual IdPtr<Turf> GetTurf();
    virtual void SetTurf(IdPtr<Turf> turf);
    IdPtr<MapObject> GetRoot();
    void PlaySoundIfVisible(const QString& name);
protected:
    virtual quint32 GetItemImpl(unsigned int hash) { return 0; }
private:
    IdPtr<MapObject> KV_SAVEABLE(owner_);
};
END_DECLARE(MapObject);

}
