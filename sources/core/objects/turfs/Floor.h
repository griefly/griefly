#pragma once

#include "Turf.h"

namespace kv
{

class Floor : public Turf
{
public:
    DECLARE_SAVEABLE(Floor, Turf);
    REGISTER_CLASS_AS(Floor);
    Floor();
    virtual void AfterWorldCreation() override;

    virtual void AttackBy(IdPtr<Item> item) override;
    void SetOpen(bool open);

    virtual void ApplyFire(int intensity) override;

    bool IsBloody() const { return bloody_; }
    void SetBloody(bool bloody) { bloody_ = bloody; }

protected:
    bool KV_SAVEABLE(open_);
    QString KV_SAVEABLE(floor_type_);

    bool KV_SAVEABLE(bloody_);
};
END_DECLARE(Floor);

class Plating : public Floor
{
public:
    DECLARE_SAVEABLE(Plating, Floor);
    REGISTER_CLASS_AS(Plating);
    Plating();
};
END_DECLARE(Plating);

}
