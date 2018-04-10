#include "Door.h"

#include "movable/Movable.h"
#include "mobs/Mob.h"
#include "movable/items/Item.h"
#include "movable/items/Weldingtool.h"

#include "test/RemoteDoorOpener.h"

using namespace kv;

Door::Door()
{
    SetTransparency(true);
    SetPassable(Dir::ALL, passable::EMPTY);

    SetVisibleLevel(10);

    door_state_ = CLOSED;

    SetSprite("icons/Doorglass.dmi");
    SetState("door_closed");

    SetName("Door");
}

void Door::Open()
{
    if (!IsState(CLOSED))
    {
        return;
    }
    SetState("door_opening");
    PlaySoundIfVisible("airlock.wav");
    door_state_ = OPENING;
    last_tick_ = GetGameTick();
    SetFreq(1);
    SetVisibleLevel(10);
}

void Door::Close()
{
    if (!IsState(OPEN))
    {
        return;
    }
    SetState("door_closing");
    PlaySoundIfVisible("airlock.wav");
    SetPassable(Dir::ALL, passable::EMPTY);
    door_state_ = CLOSING;
    last_tick_ = GetGameTick();
    SetVisibleLevel(10);
}

void Door::Process()
{
    if (IsState(OPENING))
    {
        if (GetGameTick() - last_tick_ > 11)
        {
            door_state_ = OPEN;
            SetPassable(Dir::ALL, passable::FULL);
            last_tick_ = GetGameTick();
            SetState("door_open");
            SetVisibleLevel(4);
        }
        return;
    }
    if (IsState(CLOSING))
    {
        if (GetGameTick() - last_tick_ > 11)
        {
            door_state_ = CLOSED;
            last_tick_ = GetGameTick();
            SetState("door_closed");
            SetFreq(0);
        }
        return;
    }
    if (IsState(OPEN))
    {
        if (GetGameTick() - last_tick_ > 50)
        {
            Close();
        }
    }
}

void Door::Bump(const Vector& vector, IdPtr<Movable> item)
{
    if (IdPtr<Mob> mob = item)
    {
        if (IsState(CLOSED))
        {
            Open();
        }
    }
}

void Door::Weld()
{
    if (   !IsState(CLOSED)
        && !IsState(WELDED))
    {
        return;
    }

    if (IsState(WELDED))
    {
        SetState("door_closed");
        door_state_ = CLOSED;
        GetView().RemoveOverlays();
    }
    else
    {
        GetView().AddOverlay("icons/Doorglass.dmi", "welded");
        door_state_ = WELDED;
    }
    PlaySoundIfVisible("Welder.wav");
}

void Door::AttackBy(IdPtr<Item> item)
{
    if (IdPtr<Weldingtool> welding_tool = item)
    {
        if ((IsState(CLOSED) || IsState(WELDED)) && welding_tool->Working())
        {
            Weld();
        }
        return;
    }

    if (IdPtr<RemoteDoorOpener> opener = item)
    {
        opener->SetDoor(GetId());
        return;
    }

    if (IsState(OPEN))
    {
        Close();
    }
    else if (IsState(CLOSED))
    {
        Open();
    }
}

SecurityDoor::SecurityDoor()
{
    SetSprite("icons/Doorsecglass.dmi");
}


NontransparentDoor::NontransparentDoor()
{
    SetSprite("icons/Doorsec.dmi");
    SetTransparency(false);
}

void NontransparentDoor::Open()
{
    if (!IsState(CLOSED))
    {
        return;
    }
    SetTransparency(true);
    Door::Open();
}

void NontransparentDoor::Close()
{
    if (!IsState(OPEN))
    {
        return;
    }
    SetTransparency(false);
    Door::Close();
}

ExternalDoor::ExternalDoor()
{
    SetSprite("icons/Doorext.dmi");
}

MaintenanceDoor::MaintenanceDoor()
{
    SetSprite("icons/Doormaint.dmi");
}

GlassDoor::GlassDoor()
{
    SetTransparency(true);

    SetVisibleLevel(10);

    door_prefix_ = "left";

    SetAnchored(true);

    door_state_ = CLOSED;

    SetSprite("icons/windoor.dmi");
    SetState(door_prefix_);

    SetName("Glass door");
}

void GlassDoor::AfterWorldCreation()
{
    Movable::AfterWorldCreation();

    SetPassable(GetDir(), passable::EMPTY);
    SetState(door_prefix_);
}

void GlassDoor::Open()
{
    if (!IsState(CLOSED))
    {
        return;
    }
    SetState(door_prefix_ + "opening");
    PlaySoundIfVisible("windowdoor.wav");
    door_state_ = OPENING;
    last_tick_ = GetGameTick();
    SetFreq(1);
}

void GlassDoor::Close()
{
    if (!IsState(OPEN))
    {
        return;
    }
    SetState(door_prefix_ + "closing");
    PlaySoundIfVisible("windowdoor.wav");
    SetPassable(GetDir(), passable::EMPTY);
    door_state_ = CLOSING;
    last_tick_ = GetGameTick();
}

void GlassDoor::Process()
{
    if (IsState(OPENING))
    {
        if (GetGameTick() - last_tick_ > 9)
        {
            door_state_ = OPEN;
            SetPassable(GetDir(), passable::FULL);
            last_tick_ = GetGameTick();
            SetState(door_prefix_ + "open");
        }
        return;
    }
    if (IsState(CLOSING))
    {
        if (GetGameTick() - last_tick_ > 9)
        {
            door_state_ = CLOSED;
            last_tick_ = GetGameTick();
            SetState(door_prefix_);
            SetFreq(0);
        }
        return;
    }
    if (IsState(OPEN))
    {
        if (GetGameTick() - last_tick_ > 50)
        {
            Close();
        }
    }
}

void GlassDoor::Bump(const Vector& vector, IdPtr<Movable> item)
{
    if (IdPtr<Mob> mob = item)
    {
        if (IsState(CLOSED))
        {
            Open();
        }
    }
}

void GlassDoor::AttackBy(IdPtr<Item> item)
{
    if (IsState(OPEN))
    {
        Close();
    }
    else
    {
        Open();
    }
}
