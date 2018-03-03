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

    static const int FORCE_UNIT = 100;

    PhysicsEngine();
    void AfterWorldCreation() override;

    void ProcessPhysics();

    void Add(IdPtr<Movable> movable);

    // TODO: base force vector value for movement on 1 tile should more than 1.
    // So force vectors values should be scaled up
    static Dir ProcessForceTick(Vector* force, qint32* progress, qint32 max_progress, int friction, int mass);
    static void ApplyForce(Vector* force, qint32* progress, qint32* max_progress, const Vector& addition);
private:
    void Clear();

    QVector<IdPtr<Movable>> KV_SAVEABLE(under_force_);
    // TODO: remove with Clear function
    QVector<IdPtr<Movable>> KV_SAVEABLE(to_add_);
};
END_DECLARE(PhysicsEngine);

}
