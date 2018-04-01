#include "RemoteDoorOpener.h"

using namespace kv;

RemoteDoorOpener::RemoteDoorOpener()
{
    SetSprite("icons/device.dmi");
    SetState("multitool_red");
    SetName("Atmos tool");

    door_ = 0;
}

void RemoteDoorOpener::AttackBy(IdPtr<Item> item)
{
    if (item.Id() == GetId())
    {
        if (door_.IsValid())
        {
            door_->Open();
        }
    }
}

void RemoteDoorOpener::SetDoor(IdPtr<Door> door)
{
    door_ = door;
}
