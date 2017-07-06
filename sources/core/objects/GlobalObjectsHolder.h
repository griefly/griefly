#pragma once

#include "Object.h"
#include "core/SaveableOperators.h"

namespace kv
{
class UnsyncGenerator;
class Map;
class SynchronizedRandom;
class Lobby;
class PhysicsEngine;

class GlobalObjectsHolder : public Object
{
public:
    DECLARE_SAVEABLE(GlobalObjectsHolder, Object);
    REGISTER_CLASS_AS(GlobalObjectsHolder);
    GlobalObjectsHolder();

    qint32 KV_SAVEABLE(game_tick);

    IdPtr<UnsyncGenerator> KV_SAVEABLE(unsync_generator);
    IdPtr<Map> KV_SAVEABLE(map);
    IdPtr<SynchronizedRandom> KV_SAVEABLE(random);
    IdPtr<Lobby> KV_SAVEABLE(lobby);
    IdPtr<PhysicsEngine> KV_SAVEABLE(physics_engine_);

    QMap<quint32, QPair<QString, int>> KV_SAVEABLE(musics_for_mobs);
    QMap<quint32, quint32> KV_SAVEABLE(players_table);

    QVector<IdPtr<Object>> KV_SAVEABLE(hearers);
};
END_DECLARE(GlobalObjectsHolder);

}

