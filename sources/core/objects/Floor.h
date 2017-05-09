#pragma once

#include "Turf.h"

namespace kv
{

class Floor : public ITurf
{
public:
    DECLARE_SAVED(Floor, ITurf);
    DECLARE_GET_TYPE_ITEM(Floor);
    Floor();
    virtual void AfterWorldCreation() override;

    virtual void AttackBy(IdPtr<kv::Item> item) override;
    void SetOpen(bool o);

    bool KV_SAVEBLE(bloody);
protected:
    bool KV_SAVEBLE(open_);
    QString KV_SAVEBLE(floor_type_);
};
ADD_TO_TYPELIST(Floor);

class Plating : public Floor
{
public:
    DECLARE_SAVED(Plating, Floor);
    DECLARE_GET_TYPE_ITEM(Plating);
    Plating();
};
ADD_TO_TYPELIST(Plating);

}
