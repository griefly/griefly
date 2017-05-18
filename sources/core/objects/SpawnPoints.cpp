#include "SpawnPoints.h"

#include "Lobby.h"

using namespace kv;

SpawnPoint::SpawnPoint()
{
    SetSprite("icons/screen1.dmi");
    SetState("x");

    spawn_type = "default";
}

void SpawnPoint::AfterWorldCreation()
{
    MaterialObject::AfterWorldCreation();

    SetState("");

    //GetLobby().AddSpawnPoint(GetId());
}
