#pragma once

#include "MainInt.h"
#include "SpawnPoints.h"
#include "TileInt.h"

class Lobby: public IMainObject
{
public:
    DECLARE_SAVED(Lobby, IMainObject);
    DECLARE_GET_TYPE_ITEM(Lobby)
    Lobby(size_t id);
    int GetSecondUntilStart();
    virtual void process() override;

    void AddSpawnPoint(id_ptr_on<SpawnPoint> point);
    std::vector<id_ptr_on<CubeTile>> GetTilesFor(const std::string& type);
private:
    int KV_SAVEBLE(seconds_);
    std::vector<id_ptr_on<SpawnPoint>> KV_SAVEBLE(spawn_points_);

    KV_ON_LOAD_CALL(SetThisAsLobby);
    void SetThisAsLobby();
};
ADD_TO_TYPELIST(Lobby);

void SetLobby(Lobby* lobby);
Lobby& GetLobby();
