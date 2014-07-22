#include "Door.h"

#include "sound.h"
#include "IMovable.h"
#include "MobInt.h"
#include "Item.h"
#include "Weldingtool.h"

Door::Door()
{
    transparent = true;
    SetPassable(D_ALL, false);

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
    SetPassable(D_ALL, false);
    door_state_ = CLOSING;
    last_tick_ = MAIN_TICK;
}

void Door::process()
{
    if (door_state_ == OPENING)
    {
        if (MAIN_TICK - last_tick_ > 11)
        {
            door_state_ = OPEN;
            SetPassable(D_ALL, true);
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
    }
    else
    {
        SetState("welded");
        door_state_ = WELDED;
    }
}

void Door::AttackBy(id_ptr_on<Item> item)
{
    if (id_ptr_on<Weldingtool> w = item)
    {
        if (w->Working())
            Weld();
        return;
    }


    if (IsOpen())
        Close();
    else
        Open();
}