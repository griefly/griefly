#pragma once

#include "MainObject.h"
#include "SpawnPoints.h"
#include "Tile.h"

namespace kv
{

class Lobby : public Object
{
public:
    DECLARE_SAVEABLE(Lobby, Object);
    REGISTER_CLASS_AS(Lobby);
    Lobby();
    virtual void AfterWorldCreation() override;

    int GetSecondUntilStart();
    virtual void Process() override;

    void AddSpawnPoint(IdPtr<SpawnPoint> PosPoint);
    std::vector<IdPtr<CubeTile>> GetTilesFor(const QString& type);
private:
    int KV_SAVEABLE(seconds_);
    std::vector<IdPtr<SpawnPoint>> KV_SAVEABLE(spawn_points_);

    KV_ON_LOAD_CALL(SetThisAsLobby);
    void SetThisAsLobby();
};
END_DECLARE(Lobby);

void SetLobby(Lobby* lobby);
Lobby& GetLobby();

}
