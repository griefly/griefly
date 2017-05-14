#pragma once

#include "OnMapObject.h"

namespace kv
{

class SpawnPoint : public IOnMapObject
{
public:
    DECLARE_SAVEABLE(SpawnPoint, IOnMapObject);
    REGISTER_CLASS_AS(SpawnPoint);
    SpawnPoint();

    virtual void AfterWorldCreation() override;
    QString KV_SAVEABLE(spawn_type);
};
END_DECLARE(SpawnPoint);

}
