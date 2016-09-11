#include "Door.h"

#include "representation/Sound.h"
#include "Movable.h"
#include "Mob.h"
#include "Item.h"
#include "Weldingtool.h"

Door::Door(size_t id) : IOnMapObject(id)
{
    transparent = true;
    SetPassable(D_ALL, Passable::EMPTY);

    v_level = 10;

    door_state_ = CLOSED;

    SetSprite("icons/Doorglass.dmi");
    SetState("door_closed");

    name = "Door";
}

void Door::Open()
{
    if (!CheckState(CLOSED))
    {
        return;
    }
    SetState("door_opening");
    PlaySoundIfVisible("airlock.ogg", owner.Id());
    door_state_ = OPENING;
    last_tick_ = MAIN_TICK;
    SetFreq(1);
}

void Door::Close()
{
    if (!CheckState(OPEN))
    {
        return;
    }
    SetState("door_closing");
    PlaySoundIfVisible("airlock.ogg", owner.Id());
    SetPassable(D_ALL, Passable::EMPTY);
    door_state_ = CLOSING;
    last_tick_ = MAIN_TICK;
}

void Door::Process()
{
    if (CheckState(OPENING))
    {
        if (MAIN_TICK - last_tick_ > 11)
        {
            door_state_ = OPEN;
            SetPassable(D_ALL, Passable::FULL);
            last_tick_ = MAIN_TICK;
            SetState("door_open");
        }
        return;
    }
    if (CheckState(CLOSING))
    {
        if (MAIN_TICK - last_tick_ > 11)
        {
            door_state_ = CLOSED;
            last_tick_ = MAIN_TICK;
            SetState("door_closed");
            SetFreq(0);
        }
        return;
    }
    if (CheckState(OPEN))
    {
        if (MAIN_TICK - last_tick_ > 50)
        {
            Close();
        }
    }
}

void Door::Bump(IdPtr<IMovable> item)
{
    if (IdPtr<IMob> m = item)
    {
        if (CheckState(CLOSED))
        {
            Open();
        }
    }
}

void Door::Weld()
{
    if (   !CheckState(CLOSED)
        && !CheckState(WELDED))
    {
        return;
    }

    if (CheckState(WELDED))
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
    PlaySoundIfVisible("Welder.ogg", owner.Id());
}

void Door::AttackBy(IdPtr<Item> item)
{
    if (IdPtr<Weldingtool> w = item)
    {
        if ((CheckState(CLOSED) || CheckState(WELDED)) && w->Working())
        {
            Weld();
        }
        return;
    }


    if (CheckState(OPEN))
    {
        Close();
    }
    else if (CheckState(CLOSED))
    {
        Open();
    }
}

SecurityDoor::SecurityDoor(size_t id) : Door(id)
{
    SetSprite("icons/Doorsecglass.dmi");
}


NontransparentDoor::NontransparentDoor(size_t id) : Door(id)
{
    SetSprite("icons/Doorsec.dmi");
    transparent = false;
}

void NontransparentDoor::Open()
{
    if (!CheckState(CLOSED))
    {
        return;
    }
    transparent = true;
    Door::Open();
}

void NontransparentDoor::Close()
{
    if (!CheckState(OPEN))
    {
        return;
    }
    transparent = false;
    Door::Close();
}

ExternalDoor::ExternalDoor(size_t id) : NontransparentDoor(id)
{
    SetSprite("icons/Doorext.dmi");
}

MaintenanceDoor::MaintenanceDoor(size_t id) : NontransparentDoor(id)
{
    SetSprite("icons/Doormaint.dmi");
}

GlassDoor::GlassDoor(size_t id) : IMovable(id)
{
    transparent = true;

    v_level = 10;

    door_prefix_ = "left";

    anchored = true;

    door_state_ = CLOSED;

    SetSprite("icons/windoor.dmi");
    SetState(door_prefix_);

    name = "Glass door";
}

void GlassDoor::AfterWorldCreation()
{
    IMovable::AfterWorldCreation();

    SetPassable(GetDir(), Passable::EMPTY);
    SetState(door_prefix_);
}

void GlassDoor::Open()
{
    if (!CheckState(CLOSED))
    {
        return;
    }
    SetState(door_prefix_ + "opening");
    PlaySoundIfVisible("windowdoor.ogg", owner.Id());
    door_state_ = OPENING;
    last_tick_ = MAIN_TICK;
    SetFreq(1);
}

void GlassDoor::Close()
{
    if (!CheckState(OPEN))
    {
        return;
    }
    SetState(door_prefix_ + "closing");
    PlaySoundIfVisible("windowdoor.ogg", owner.Id());
    SetPassable(GetDir(), Passable::EMPTY);
    door_state_ = CLOSING;
    last_tick_ = MAIN_TICK;
}

void GlassDoor::Process()
{
    if (CheckState(OPENING))
    {
        if (MAIN_TICK - last_tick_ > 9)
        {
            door_state_ = OPEN;
            SetPassable(GetDir(), Passable::FULL);
            last_tick_ = MAIN_TICK;
            SetState(door_prefix_ + "open");
        }
        return;
    }
    if (CheckState(CLOSING))
    {
        if (MAIN_TICK - last_tick_ > 9)
        {
            door_state_ = CLOSED;
            last_tick_ = MAIN_TICK;
            SetState(door_prefix_);
            SetFreq(0);
        }
        return;
    }
    if (CheckState(OPEN))
    {
        if (MAIN_TICK - last_tick_ > 50)
        {
            Close();
        }
    }
}

void GlassDoor::Bump(IdPtr<IMovable> item)
{
    if (IdPtr<IMob> m = item)
    {
        if (CheckState(CLOSED))
        {
            Open();
        }
    }
}

void GlassDoor::AttackBy(IdPtr<Item> item)
{
    if (CheckState(OPEN))
    {
        Close();
    }
    else
    {
        Open();
    }
}
