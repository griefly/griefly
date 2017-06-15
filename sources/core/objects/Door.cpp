#include "Door.h"

#include "representation/Sound.h"

#include "movable/Movable.h"
#include "mobs/Mob.h"
#include "movable/items/Item.h"
#include "movable/items/Weldingtool.h"

#include "test/RemoteDoorOpener.h"

using namespace kv;

Door::Door()
{
    transparent = true;
    SetPassable(Dir::ALL, passable::EMPTY);

    v_level = 10;

    door_state_ = CLOSED;

    SetSprite("icons/Doorglass.dmi");
    SetState("door_closed");

    name = "Door";
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

void Door::Bump(IdPtr<Movable> item)
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
        GetView()->RemoveOverlays();
    }
    else
    {
        GetView()->AddOverlay("icons/Doorglass.dmi", "welded");
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
    transparent = false;
}

void NontransparentDoor::Open()
{
    if (!IsState(CLOSED))
    {
        return;
    }
    transparent = true;
    Door::Open();
}

void NontransparentDoor::Close()
{
    if (!IsState(OPEN))
    {
        return;
    }
    transparent = false;
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
    transparent = true;

    v_level = 10;

    door_prefix_ = "left";

    anchored_ = true;

    door_state_ = CLOSED;

    SetSprite("icons/windoor.dmi");
    SetState(door_prefix_);

    name = "Glass door";
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

void GlassDoor::Bump(IdPtr<Movable> item)
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
