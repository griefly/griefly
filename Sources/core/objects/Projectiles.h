#pragma once

#include "Movable.h"
#include "Human.h"


class Projectile : public IMovable
{
public:
    Projectile(size_t id);
    DECLARE_SAVED(Projectile,IMovable)	
    DECLARE_GET_TYPE_ITEM(Projectile)
    void MakeMovementPattern(VDir target,id_ptr_on<Human> x);
    void Process();
    virtual void Bump(id_ptr_on<IMovable> item) override;
    virtual void BumpByGas(Dir dir, bool inside = false) override {};
    virtual bool CheckPassable() override;
    virtual void AfterWorldCreation() override;
    void MakeMovementLoops(int straight,int diagonal,VDir Direction,VDir Direction2);
    Dir CalculateTrajectoryMove();
    int KV_SAVEBLE(damage);
private:
    VDir KV_SAVEBLE(target_);
    bool KV_SAVEBLE(reached_target);
    int KV_SAVEBLE(set_target_);
    Dir KV_SAVEBLE(direction);
    int KV_SAVEBLE(cases_);
    id_ptr_on<Human> KV_SAVEBLE(shooter_);
    int KV_SAVEBLE(got_bored_);
};
ADD_TO_TYPELIST(Projectile);
