#pragma once

#include "OnMapObject.h"

class SpawnPoint: public IOnMapObject
{
public:
    DECLARE_SAVED(SpawnPoint, IOnMapObject);
    DECLARE_GET_TYPE_ITEM(SpawnPoint);
    SpawnPoint(size_t id);

    virtual void AfterWorldCreation() override;
    std::string KV_SAVEBLE(spawn_type);
};
ADD_TO_TYPELIST(SpawnPoint);
