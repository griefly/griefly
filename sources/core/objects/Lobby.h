#pragma once

#include "Object.h"
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

    void AddSpawnPoint(IdPtr<SpawnPoint> Position);
    std::vector<IdPtr<CubeTile>> GetTilesFor(const QString& type);
private:
    int KV_SAVEABLE(seconds_);
    std::vector<IdPtr<SpawnPoint>> KV_SAVEABLE(spawn_points_);
};
END_DECLARE(Lobby);
}
