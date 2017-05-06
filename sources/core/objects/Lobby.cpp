#include "Lobby.h"

Lobby* single_lobby = nullptr;
void SetLobby(Lobby* lobby)
{
    single_lobby = lobby;
}

Lobby& GetLobby()
{
    return *single_lobby;
}

Lobby::Lobby()
{
    seconds_ = 1;//60 * 2;
}

void Lobby::AfterWorldCreation()
{
    Object::AfterWorldCreation();
    SetFreq(10);
    SetThisAsLobby();
}

int Lobby::GetSecondUntilStart()
{
    return seconds_;
}

void Lobby::Process()
{
    --seconds_;
}

void Lobby::AddSpawnPoint(IdPtr<SpawnPoint> point)
{
    spawn_points_.push_back(point);
}

std::vector<IdPtr<CubeTile>> Lobby::GetTilesFor(const QString& type)
{
    std::vector<IdPtr<CubeTile>> retval;
    for (auto it = spawn_points_.begin(); it != spawn_points_.end(); ++it)
    {
        if ((*it)->spawn_type == type)
        {
            retval.push_back((*it)->GetOwner());
        }
    }
    return retval;
}

void Lobby::SetThisAsLobby()
{
    SetLobby(this);
}
