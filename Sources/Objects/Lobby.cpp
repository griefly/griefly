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

void Lobby::SetThisAsLobby()
{
    SetLobby(this);
}
