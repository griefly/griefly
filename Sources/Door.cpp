#include "Door.h"

#include "sound.h"

Door::Door()
{
    transparent = true;
    passable = false;

    v_level = 10;

    open_ = false;

    SetSprite("icons/Doorglass.dmi");
    SetState("door_closed");

    name = "Door";
}

void Door::Open()
{
    if (open_ == true)
        return;
    SetState("door_opening");
    PlaySoundIfVisible("airlock.ogg", owner);
    passable = true;
    open_ = true;
}

void Door::Close()
{
    if (open_ == false)
        return;
    SetState("door_closing");
    PlaySoundIfVisible("airlock.ogg", owner);
    passable = false;
    open_ = false;  
}