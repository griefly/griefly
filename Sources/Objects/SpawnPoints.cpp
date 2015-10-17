#include "SpawnPoints.h"

#include "Lobby.h"

SpawnPoint::SpawnPoint(size_t id) : IOnMapObject(id)
{
    SetSprite("icons/screen1.dmi");
    SetState("x");

    spawn_type = "default";
}

void SpawnPoint::AfterWorldCreation()
{
    IOnMapObject::AfterWorldCreation();

    SetState("");

    //GetLobby().AddSpawnPoint(GetId());
}
