#include "MainObject.h"
#include "../Map.h"
#include "../ObjectFactory.h"
#include "../Game.h"
#include "representation/Representation.h"

void IMainObject::PlayMusic(const QString& name, float volume)
{
    GetGame().PlayMusic(name, volume);
}

void IMainObject::Delete()
{
    GetGame().GetFactory().DeleteLater(id_);
}

void IMainObject::Save(FastSerializer& serializer)
{
    serializer.WriteType(GetType());
    serializer << id_;
    serializer << how_often_;
}

void IMainObject::Load(FastDeserializer& deserializer)
{
    // It is mainly empty because all data is loaded by
    // the outer function

    deserializer >> how_often_;

    SetFreq(GetFreq());
}

void IMainObject::SetFreq(int freq)
{
    if (game_ == nullptr)
    {
        KvAbort("SetFreq is called in constructor");
    }

    how_often_ = freq;

    // TODO: Why is it here?
    if (!GetId())
    {
        KvAbort("GetId() is zero");
    }

    if (how_often_ != 0)
    {
        GetGame().GetFactory().AddProcessingItem(GetId());
    }
}

const GameInterface& IMainObject::GetGame() const
{
    if (game_ == nullptr)
    {
        KvAbort("IMainObject::GetGame() is called during construction of object");
    }
    return *game_;
}

ObjectFactoryInterface& IMainObject::GetFactory()
{
    return GetGame().GetFactory();
}

GameInterface& IMainObject::GetGame()
{
    if (game_ == nullptr)
    {
        KvAbort("IMainObject::GetGame() is called during construction of object");
    }
    return *game_;
}

unsigned int IMainObject::GetRand()
{
    return GetGame().GetRandom().GetRand();
}

quint32 IMainObject::CreateImpl(const QString& type, quint32 owner)
{
    return GetFactory().CreateImpl(type, owner);
}
