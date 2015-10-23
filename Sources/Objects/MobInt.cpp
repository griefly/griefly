#include "MobInt.h"
#include "Manager.h"
#include "MapClass.h"

#include "MagicStrings.h"
#include "Creator.h"


#include "NetworkMessagesTypes.h"

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

void IMob::processGUImsg(const Message2 &msg)
{
    if (msg.type != MessageType::ORDINARY)
        return;

    QJsonObject obj = Network2::ParseJson(msg);

    if (Network2::IsKey(obj, Input::MOVE_UP))
        checkMove(D_UP);
    else if (Network2::IsKey(obj, Input::MOVE_DOWN))
        checkMove(D_DOWN);
    else if (Network2::IsKey(obj, Input::MOVE_LEFT))
        checkMove(D_LEFT);
    else if (Network2::IsKey(obj, Input::MOVE_RIGHT))
        checkMove(D_RIGHT);
}

void IMob::processPhysics()
{
    IMessageReceiver::processPhysics();
    if(GetId() == GetMob().ret_id())
        GetManager().UpdateVisible();
}
