#pragma once

#include "OnMapObject.h"

class WallSign: public IOnMapObject
{
public:
    DECLARE_SAVED(WallSign, IOnMapObject);
    DECLARE_GET_TYPE_ITEM(WallSign);
    WallSign();
    virtual void AfterWorldCreation() override;

    virtual void AttackBy(IdPtr<Item> item) override;
protected:
    QString KV_SAVEBLE(sign_type_);
};
ADD_TO_TYPELIST(WallSign);

