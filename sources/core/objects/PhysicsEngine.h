#pragma once

#include <QVector>

#include "core/Idptr.h"
#include "Object.h"

#include "core/SaveableOperators.h"

namespace kv
{

class Movable;

class PhysicsEngine : public Object
{
public:
    DECLARE_SAVEABLE(PhysicsEngine, Object);
    REGISTER_CLASS_AS(PhysicsEngine);

    PhysicsEngine();
    void AfterWorldCreation() override;

    void Process();

    void Add(IdPtr<Movable> movable);
private:
    void Clear();

    QVector<IdPtr<Movable>> KV_SAVEABLE(under_force_);
    // TODO: remove with Clear function
    QVector<IdPtr<Movable>> KV_SAVEABLE(to_add_);
};
END_DECLARE(PhysicsEngine);

}
