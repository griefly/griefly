#pragma once

#include "MaterialObject.h"

namespace kv
{

class SpawnPoint : public MaterialObject
{
public:
    DECLARE_SAVEABLE(SpawnPoint, MaterialObject);
    REGISTER_CLASS_AS(SpawnPoint);
    SpawnPoint();

    virtual void AfterWorldCreation() override;
    QString KV_SAVEABLE(spawn_type);
};
END_DECLARE(SpawnPoint);

}
