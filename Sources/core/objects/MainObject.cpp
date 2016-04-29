#include "MainObject.h"
#include "../Map.h"
#include "../ObjectFactory.h"
#include "../Game.h"
#include "representation/Representation.h"

void IMainObject::PlaySoundIfVisible(const std::string& name, size_t tile_id)
{
    if (game_->GetMap().IsTileVisible(tile_id))
    {
        GetRepresentation().AddToNewFrame(name);
    }
}


void IMainObject::PlayMusic(const std::string& name, float volume)
{
    game_->PlayMusic(name, volume);
}

void IMainObject::Delete()
{
    game_->GetFactory().GetIdTable()[id_] = nullptr;
    delete this;
}

bool IMainObject::saveSelf(std::stringstream& file)
{
    file << " " << T_ITEM() << " ";
    file << " " << id_ << " ";
    file << " " << how_often_ << " ";
    return true;
}

bool IMainObject::loadSelf(std::stringstream& file)
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
        return;

    if (how_often_ != 0)
    {
        game_->GetFactory().AddProcessingItem(GetId());
    }
}

const Game& IMainObject::GetGame() const
{
    if (game_ == nullptr)
    {
        qDebug() << "IMainObject::GetGame() is called during construction of object";
        abort();
    }
    return *game_;
}

Game& IMainObject::GetGame()
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

void IMainObject::SetId(size_t id)
{
    id_ = id;
    SetFreq(GetFreq());
}
