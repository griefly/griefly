#include "Mob.h"

#include "core/Map.h"

#include "client/net/MagicStrings.h"

using namespace kv;

Mob::Mob()
{
    // Nothing
}

void Mob::AfterWorldCreation()
{
    SetFreq(1);
}

void Mob::GenerateInterfaceForFrame(GrowingFrame* representation)
{
}

void Mob::Delete()
{
    MindExit();
    MaterialObject::Delete();
}

void Mob::ProcessMessage(const Message& message)
{
    if (message.type != message_type::ORDINARY)
    {
        return;
    }

    if (IsKey(message.data, Input::MOVE_UP))
    {
        TryMove(Dir::NORTH);
    }
    else if (IsKey(message.data, Input::MOVE_DOWN))
    {
        TryMove(Dir::SOUTH);
    }
    else if (IsKey(message.data, Input::MOVE_LEFT))
    {
        TryMove(Dir::WEST);
    }
    else if (IsKey(message.data, Input::MOVE_RIGHT))
    {
        TryMove(Dir::EAST);
    }
}

void Mob::MoveMindTo(IdPtr<Mob> other)
{
    quint32 net_id = GetGame().GetNetId(GetId());
    if (net_id == 0)
    {
        return;
    }

    MindExit();

    GetGame().SetPlayerId(net_id, other.Id());
    if (GetId() == GetGame().GetMob().Id())
    {
        GetGame().SetMob(other.Id());
    }

    other->MindEnter();
}

bool Mob::IsMinded() const
{
    quint32 net_id = GetGame().GetNetId(GetId());
    if (net_id == 0)
    {
        return false;
    }
    return true;
}
