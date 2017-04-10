#include "Mob.h"
#include "../Game.h"
#include "../Map.h"

#include "net/MagicStrings.h"
#include "net/NetworkMessagesTypes.h"

IMob::IMob(quint32 id) : IMessageReceiver(id)
{
    // Nothing
}

void IMob::AfterWorldCreation()
{
    SetFreq(1);
}

void IMob::cautOverMind()
{
}

void IMob::GenerateInterfaceForFrame()
{
}

void IMob::Delete()
{
    DeinitGUI();
    cautOverMind();
    IOnMapObject::Delete();
}

void IMob::ProcessMessage(const Message2 &msg)
{
    if (msg.type != MessageType::ORDINARY)
    {
        return;
    }

    QJsonObject obj = Network2::ParseJson(msg);

    if (Network2::IsKey(obj, Input::MOVE_UP))
    {
        TryMove(Dir::UP);
    }
    else if (Network2::IsKey(obj, Input::MOVE_DOWN))
    {
        TryMove(Dir::DOWN);
    }
    else if (Network2::IsKey(obj, Input::MOVE_LEFT))
    {
        TryMove(Dir::LEFT);
    }
    else if (Network2::IsKey(obj, Input::MOVE_RIGHT))
    {
        TryMove(Dir::RIGHT);
    }
}
