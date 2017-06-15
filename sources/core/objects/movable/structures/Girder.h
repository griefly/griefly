#pragma once

#include "Structure.h"

namespace kv
{

class Girder : public Structure
{
public:
    DECLARE_SAVEABLE(Girder, Structure);
    REGISTER_CLASS_AS(Girder);
    Girder();
    virtual void AttackBy(IdPtr<Item> item) override;
};
END_DECLARE(Girder);

}
