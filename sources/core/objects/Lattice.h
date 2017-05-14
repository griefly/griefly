#pragma once

#include "Structure.h"

namespace kv
{

class Lattice : public Structure
{
public:
    DECLARE_SAVEABLE(Lattice, Structure);
    REGISTER_CLASS_AS(Lattice);
    Lattice();
    virtual void AttackBy(IdPtr<Item> item) override;
};
END_DECLARE(Lattice);

}
