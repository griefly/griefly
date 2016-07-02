#pragma once

#include "MainObject.h"
#include "SpawnPoints.h"
#include "Tile.h"

class Lobby: public IMainObject
{
public:
    DECLARE_SAVED(Lobby, IMainObject);
    DECLARE_GET_TYPE_ITEM(Lobby)
    Lobby(size_t id);
    virtual void AfterWorldCreation() override;

    int GetSecondUntilStart();
    virtual void Process() override;

    void AddSpawnPoint(id_ptr_on<SpawnPoint> PosPoint);
    std::vector<id_ptr_on<CubeTile>> GetTilesFor(const std::string& type);

    int KV_SAVEBLE(security_score_);
    int KV_SAVEBLE(janitors_score_);
private:
    int KV_SAVEBLE(seconds_);
    std::vector<id_ptr_on<SpawnPoint>> KV_SAVEBLE(spawn_points_);

    KV_ON_LOAD_CALL(SetThisAsLobby);
    void SetThisAsLobby();
};
ADD_TO_TYPELIST(Lobby);

void SetLobby(Lobby* lobby);
Lobby& GetLobby();
