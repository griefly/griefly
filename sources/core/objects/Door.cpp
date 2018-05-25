#include "Door.h"

#include "movable/Movable.h"
#include "mobs/Mob.h"
#include "movable/items/Item.h"
#include "movable/items/Weldingtool.h"

#include "test/RemoteDoorOpener.h"

using namespace kv;

namespace
{

const int CLOSED_DOOR = visible_level::STANDING_MOB + 1;
const int OPEN_DOOR = visible_level::ITEM - 1;

}

Door::Door()
{
    SetTransparency(true);
    SetPassable(Dir::ALL, passable::EMPTY);

    SetVisibleLevel(OPEN_DOOR);

    door_state_ = State::CLOSED;

    SetSprite("icons/Doorglass.dmi");
    SetState("door_closed");

    SetName("Door");
}

void Door::Open()
{
    if (!IsState(State::CLOSED))
    {
        return;
    }
    SetState("door_opening");
    PlaySoundIfVisible("airlock.wav");
    door_state_ = State::OPENING;
    last_tick_ = GetGameTick();
    SetFreq(1);
    SetVisibleLevel(CLOSED_DOOR);
}

void Door::Close()
{
    if (!IsState(State::OPEN))
    {
        return;
    }
    SetState("door_closing");
    PlaySoundIfVisible("airlock.wav");
    SetPassable(Dir::ALL, passable::EMPTY);
    door_state_ = State::CLOSING;
    last_tick_ = GetGameTick();
    SetVisibleLevel(CLOSED_DOOR);
}

void Door::Process()
{
    if (IsState(State::OPENING))
    {
        if (GetGameTick() - last_tick_ > 11)
        {
            door_state_ = State::OPEN;
            SetPassable(Dir::ALL, passable::FULL);
            last_tick_ = GetGameTick();
            SetState("door_open");
            SetVisibleLevel(OPEN_DOOR);
        }
        return;
    }
    if (IsState(State::CLOSING))
    {
        if (GetGameTick() - last_tick_ > 11)
        {
            door_state_ = State::CLOSED;
            last_tick_ = GetGameTick();
            SetState("door_closed");
            SetFreq(0);
        }
        return;
    }
    if (IsState(State::OPEN))
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
        if (IsState(State::CLOSED))
        {
            Open();
        }
    }
}

void Door::Weld()
{
    if (   !IsState(State::CLOSED)
        && !IsState(State::WELDED))
    {
        return;
    }

    if (IsState(State::WELDED))
    {
        SetState("door_closed");
        door_state_ = State::CLOSED;
        GetView().RemoveOverlays();
    }
    else
    {
        GetView().AddOverlay("icons/Doorglass.dmi", "welded");
        door_state_ = State::WELDED;
    }
    PlaySoundIfVisible("Welder.wav");
}

void Door::AttackBy(IdPtr<Item> item)
{
    if (IdPtr<Weldingtool> welding_tool = item)
    {
        if ((IsState(State::CLOSED) || IsState(State::WELDED)) && welding_tool->IsWorking())
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

    if (IsState(State::OPEN))
    {
        Close();
    }
    else if (IsState(State::CLOSED))
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
    if (!IsState(State::CLOSED))
    {
        return;
    }
    SetTransparency(true);
    Door::Open();
}

void NontransparentDoor::Close()
{
    if (!IsState(State::OPEN))
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

    SetVisibleLevel(CLOSED_DOOR);

    door_prefix_ = "left";

    SetAnchored(true);

    door_state_ = State::CLOSED;

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
    if (!IsState(State::CLOSED))
    {
        return;
    }
    SetState(door_prefix_ + "opening");
    PlaySoundIfVisible("windowdoor.wav");
    door_state_ = State::OPENING;
    last_tick_ = GetGameTick();
    SetFreq(1);
}

void GlassDoor::Close()
{
    if (!IsState(State::OPEN))
    {
        return;
    }
    SetState(door_prefix_ + "closing");
    PlaySoundIfVisible("windowdoor.wav");
    SetPassable(GetDir(), passable::EMPTY);
    door_state_ = State::CLOSING;
    last_tick_ = GetGameTick();
}

void GlassDoor::Process()
{
    if (IsState(State::OPENING))
    {
        if (GetGameTick() - last_tick_ > 9)
        {
            door_state_ = State::OPEN;
            SetPassable(GetDir(), passable::FULL);
            last_tick_ = GetGameTick();
            SetState(door_prefix_ + "open");
        }
        return;
    }
    if (IsState(State::CLOSING))
    {
        if (GetGameTick() - last_tick_ > 9)
        {
            door_state_ = State::CLOSED;
            last_tick_ = GetGameTick();
            SetState(door_prefix_);
            SetFreq(0);
        }
        return;
    }
    if (IsState(State::OPEN))
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
        if (IsState(State::CLOSED))
        {
            Open();
        }
    }
}

void GlassDoor::AttackBy(IdPtr<Item> item)
{
    if (IsState(State::OPEN))
    {
        Close();
    }
    else
    {
        Open();
    }
}
