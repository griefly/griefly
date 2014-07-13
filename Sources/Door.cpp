#include "Door.h"

#include "sound.h"

Door::Door()
{
    transparent = true;
    passable = false;

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
    PlaySoundIfVisible("airlock.ogg", owner);
    door_state_ = OPENING;
    last_tick_ = MAIN_TICK;
    SetFreq(1);
}

void Door::Close()
{
    if (door_state_ != OPEN)
        return;
    SetState("door_closing");
    PlaySoundIfVisible("airlock.ogg", owner);
    passable = false;
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
            passable = true;
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
        if (MAIN_TICK - last_tick_ > 100)
            Close();
}