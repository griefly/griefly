#pragma once

#include "Movable.h"

namespace kv
{

class Bed : public IMovable
{
public:
    DECLARE_SAVEABLE(Bed, IMovable);
    REGISTER_CLASS_AS(Bed);
    Bed();
};
END_DECLARE(Bed);

class Chair : public IMovable
{
public:
    DECLARE_SAVEABLE(Chair, IMovable);
    REGISTER_CLASS_AS(Chair);
    Chair();

    virtual void AfterWorldCreation() override;
};
END_DECLARE(Chair);

class Stool : public IMovable
{
public:
    DECLARE_SAVEABLE(Stool, IMovable);
    REGISTER_CLASS_AS(Stool);
    Stool();
};
END_DECLARE(Stool);

}
