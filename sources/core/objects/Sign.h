#pragma once

#include "MaterialObject.h"

namespace kv
{

class WallSign : public MaterialObject
{
public:
    DECLARE_SAVEABLE(WallSign, MaterialObject);
    REGISTER_CLASS_AS(WallSign);
    WallSign();
    virtual void AfterWorldCreation() override;
protected:
    QString KV_SAVEABLE(sign_type_);
};
END_DECLARE(WallSign);

}
