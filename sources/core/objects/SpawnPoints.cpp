#include "SpawnPoints.h"

#include "Lobby.h"

SpawnPoint::SpawnPoint()
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
