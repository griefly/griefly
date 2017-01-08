#pragma once

#include "Turf.h"

class Sign: public ITurf
{
public:
    DECLARE_SAVED(Sign, ITurf);
    DECLARE_GET_TYPE_ITEM(Sign);
    Sign(quint32 id);
    virtual void AfterWorldCreation() override;

    virtual void AttackBy(IdPtr<Item> item) override;
protected:
    QString KV_SAVEBLE(sign_type_);
};
ADD_TO_TYPELIST(Sign);

