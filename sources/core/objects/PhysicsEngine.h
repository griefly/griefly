#pragma once

#include <QVector>

#include "Idptr.h"
#include "Object.h"

#include "SaveableOperators.h"

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

    void ProcessPhysics();

    void Add(IdPtr<Movable> movable);
private:
    static Dir ProcessForceTick(Vector* force, int* progress, int friction, int mass);
    static void ApplyForce(Vector* force, int* progress, const Vector& addition);

    void Clear();

    QVector<IdPtr<Movable>> KV_SAVEABLE(under_force_);
    // TODO: remove with Clear function
    QVector<IdPtr<Movable>> KV_SAVEABLE(to_add_);
};
END_DECLARE(PhysicsEngine);

}
