#include "MainObject.h"
#include "../Map.h"
#include "../ObjectFactory.h"
#include "../Game.h"
#include "representation/Representation.h"

void IMainObject::PlaySoundIfVisible(const std::string& name, size_t tile_id)
{
    if (GetGame().GetMap().IsTileVisible(tile_id))
    {
        GetRepresentation().AddToNewFrame(name);
    }
}


void IMainObject::PlayMusic(const std::string& name, float volume)
{
    GetGame().PlayMusic(name, volume);
}

void IMainObject::Delete()
{
    GetGame().GetFactory().DeleteLater(id_);
}

bool IMainObject::SaveSelf(std::stringstream& file)
{
    file << " " << T_ITEM() << " ";
    file << " " << id_ << " ";
    file << " " << how_often_ << " ";
    return true;
}

bool IMainObject::LoadSelf(std::stringstream& file)
{
    file >> how_often_;

    SetFreq(GetFreq());

    // It empty becouse all data load main outer function
    return true;
}

void IMainObject::SetFreq(int freq)
{
    if (game_ == nullptr)
    {
        qDebug() << "SetFreq is called in constructor";
        abort();
    }

    how_often_ = freq;

    // TODO: Why is it here?
    if (!GetId())
    {
        qDebug() << "GetId() is zero";
        abort();
    }

    if (how_often_ != 0)
    {
        GetGame().GetFactory().AddProcessingItem(GetId());
    }
}

const IGame& IMainObject::GetGame() const
{
    if (game_ == nullptr)
    {
        qDebug() << "IMainObject::GetGame() is called during construction of object";
        abort();
    }
    return *game_;
}

ObjectFactory& IMainObject::GetFactory()
{
    return GetGame().GetFactory();
}

IGame& IMainObject::GetGame()
{
    if (game_ == nullptr)
    {
        qDebug() << "IMainObject::GetGame() is called during construction of object";
        abort();
    }
    return *game_;
}

unsigned int IMainObject::GetRand()
{
    return GetGame().GetRandom().GetRand();
}

size_t IMainObject::CreateImpl(const std::string& type, id_ptr_on<IOnMapBase> owner)
{
    return GetFactory().CreateImpl(type, owner);
}

void IMainObject::SetId(size_t id)
{
    id_ = id;
    SetFreq(GetFreq());
}
