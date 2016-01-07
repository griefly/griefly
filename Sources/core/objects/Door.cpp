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
    if (door_state_ != CLOSED)
        return;
    SetState("door_opening");
    PlaySoundIfVisible("airlock.ogg", owner.ret_id());
    door_state_ = OPENING;
    last_tick_ = MAIN_TICK;
    SetFreq(1);
}

void Door::Close()
{
    if (door_state_ != OPEN)
        return;
    SetState("door_closing");
    PlaySoundIfVisible("airlock.ogg", owner.ret_id());
    SetPassable(D_ALL, Passable::EMPTY);
    door_state_ = CLOSING;
    last_tick_ = MAIN_TICK;
}

void Door::Process()
{
    if (door_state_ == OPENING)
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
    if (door_state_ == CLOSING)
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
    if (door_state_ == OPEN)
        if (MAIN_TICK - last_tick_ > 50)
            Close();
}

void Door::Bump(id_ptr_on<IMovable> item)
{
    if (id_ptr_on<IMob> m = item)
    {
        if (door_state_ == CLOSED)
            Open();
    }
}

void Door::Weld()
{
    if (   door_state_ != CLOSED
        && door_state_ != WELDED)
        return;

    if (door_state_ == WELDED)
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
    PlaySoundIfVisible("Welder.ogg", owner.ret_id());
}

void Door::AttackBy(id_ptr_on<Item> item)
{
    if (id_ptr_on<Weldingtool> w = item)
    {
        if (IsClosed() && w->Working())
            Weld();
        return;
    }


    if (IsOpen())
        Close();
    else if (IsClosed())
        Open();
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
    if (!IsClosed())
    {
        return;
    }
    transparent = true;
    Door::Open();
}

void NontransparentDoor::Close()
{
    if (!IsOpen())
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
    if (door_state_ != CLOSED)
        return;
    SetState(door_prefix_ + "opening");
    PlaySoundIfVisible("windowdoor.ogg", owner.ret_id());
    door_state_ = OPENING;
    last_tick_ = MAIN_TICK;
    SetFreq(1);
}

void GlassDoor::Close()
{
    if (door_state_ != OPEN)
        return;
    SetState(door_prefix_ + "closing");
    PlaySoundIfVisible("windowdoor.ogg", owner.ret_id());
    SetPassable(GetDir(), Passable::EMPTY);
    door_state_ = CLOSING;
    last_tick_ = MAIN_TICK;
}

void GlassDoor::Process()
{
    if (door_state_ == OPENING)
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
    if (door_state_ == CLOSING)
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
    if (door_state_ == OPEN)
        if (MAIN_TICK - last_tick_ > 50)
            Close();
}

void GlassDoor::Bump(id_ptr_on<IMovable> item)
{
    if (id_ptr_on<IMob> m = item)
    {
        if (door_state_ == CLOSED)
            Open();
    }
}

void GlassDoor::AttackBy(id_ptr_on<Item> item)
{
    if (IsOpen())
        Close();
    else
        Open();
}
