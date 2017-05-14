#pragma once

#include "Turf.h"

namespace kv
{

class Space : public ITurf
{
public:
    DECLARE_SAVEABLE(Space, ITurf);
    REGISTER_CLASS_AS(Space);
    Space();
    virtual void AfterWorldCreation() override;
    virtual void AttackBy(IdPtr<Item> item) override;
};
END_DECLARE(Space);

}
