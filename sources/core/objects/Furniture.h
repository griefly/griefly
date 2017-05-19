#pragma once

#include "Movable.h"

namespace kv
{

class Bed : public Movable
{
public:
    DECLARE_SAVEABLE(Bed, Movable);
    REGISTER_CLASS_AS(Bed);
    Bed();
};
END_DECLARE(Bed);

class Chair : public Movable
{
public:
    DECLARE_SAVEABLE(Chair, Movable);
    REGISTER_CLASS_AS(Chair);
    Chair();

    virtual void AfterWorldCreation() override;
};
END_DECLARE(Chair);

class Stool : public Movable
{
public:
    DECLARE_SAVEABLE(Stool, Movable);
    REGISTER_CLASS_AS(Stool);
    Stool();
};
END_DECLARE(Stool);

}
