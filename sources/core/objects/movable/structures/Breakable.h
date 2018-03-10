#pragma once

#include "Structure.h"

namespace kv
{

class Breakable : public Structure
{
public:
    DECLARE_SAVEABLE(Breakable, Structure);
    REGISTER_CLASS_AS(Breakable);
    Breakable();

    virtual void AttackBy(IdPtr<Item> item) override;
    virtual void Bump(const Vector& vector, IdPtr<Movable> item) override;
    virtual void Break() { }
    virtual void PlayOnHitSound() { }
    void SetHitPoints(int number);
    void Hit(int damage);
private:
    int KV_SAVEABLE(hit_points_);
};
END_DECLARE(Breakable);

}
