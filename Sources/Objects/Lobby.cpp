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

Lobby::Lobby(size_t id): IMainObject(id)
{
    seconds_ = 10;//60 * 2;

    SetFreq(10);
    SetThisAsLobby();
}

int Lobby::GetSecondUntilStart()
{
    return seconds_;
}

void Lobby::process()
{
    --seconds_;
}

void Lobby::AddSpawnPoint(id_ptr_on<SpawnPoint> point)
{
    spawn_points_.push_back(point);
}

std::vector<id_ptr_on<CubeTile>> Lobby::GetTilesFor(const std::string& type)
{
    std::vector<id_ptr_on<CubeTile>> retval;
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
