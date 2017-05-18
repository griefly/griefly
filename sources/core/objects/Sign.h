#pragma once

#include "OnMapObject.h"

namespace kv
{

class WallSign : public MaterialObject
{
public:
    DECLARE_SAVEABLE(WallSign, MaterialObject);
    REGISTER_CLASS_AS(WallSign);
    WallSign();
    virtual void AfterWorldCreation() override;

    virtual void AttackBy(IdPtr<Item> item) override;
protected:
    QString KV_SAVEABLE(sign_type_);
};
END_DECLARE(WallSign);

}
