#pragma once

#include "OnMapObject.h"

class SpawnPoint : public kv::IOnMapObject
{
public:
    DECLARE_SAVED(SpawnPoint, IOnMapObject);
    DECLARE_GET_TYPE_ITEM(SpawnPoint);
    SpawnPoint();

    virtual void AfterWorldCreation() override;
    QString KV_SAVEBLE(spawn_type);
};
ADD_TO_TYPELIST(SpawnPoint);
