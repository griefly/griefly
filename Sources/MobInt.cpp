#include "MobInt.h"
#include "Mob.h"
#include "MapClass.h"
#include "Turf.h"

#include "MagicStrings.h"
#include "Creator.h"

IMob::IMob(size_t id) : IMessageReceiver(id)
{
    SetFreq(1);
    SetSprite("icons/ork.png");        
}

void IMob::cautOverMind()
{
}

void IMob::processGUI()
{
}

void IMob::delThis()
{
    DeinitGUI();
    cautOverMind();
    IOnMapObject::delThis();
}

void IMob::processGUImsg(const Message& msg)
{
    if (msg.text == Input::MOVE_UP)
        checkMove(D_UP);
    else if (msg.text == Input::MOVE_DOWN)
        checkMove(D_DOWN);
    else if (msg.text == Input::MOVE_LEFT)
        checkMove(D_LEFT);
    else if (msg.text == Input::MOVE_RIGHT)
        checkMove(D_RIGHT);
}

void IMob::processPhysics()
{
    IMessageReceiver::processPhysics();
    if(GetId() == GetMob().ret_id())
        GetManager()->UpdateVisible();
}
