#pragma once

#include <QVector>

#include "core/Idptr.h"

namespace kv
{

class Movable;

class PhysicsEngine
{
public:
    static PhysicsEngine& Get();

    void Add(IdPtr<Movable> m);
    void Process();
    void Clear();
    unsigned int Hash();
private:
    QVector<IdPtr<Movable>> under_force_;

    QVector<IdPtr<Movable>> to_add_;
};

}
