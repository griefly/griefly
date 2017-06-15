#pragma once

#include "core/objects/movable/items/Item.h"
#include "core/objects/Door.h"

namespace kv
{

class RemoteDoorOpener : public Item
{
public:
    DECLARE_SAVEABLE(RemoteDoorOpener, Item);
    REGISTER_CLASS_AS(RemoteDoorOpener);
    RemoteDoorOpener();

    virtual void AttackBy(IdPtr<Item> item) override;

    void SetDoor(IdPtr<Door> door);
private:
    IdPtr<Door> KV_SAVEABLE(door_);
};
END_DECLARE(RemoteDoorOpener)

}
