#pragma once

#include "Movable.h"


class Projectile : public IMovable
{
public:
    Projectile(size_t id);
    DECLARE_SAVED(Projectile,IMovable)	
    DECLARE_GET_TYPE_ITEM(Projectile)
    void MakeMovementPattern(VDir target);
    void Process();
    //TODO rotation
   // virtual bool Rotate(Dir dir);
    virtual void Bump(id_ptr_on<IMovable> item) override;
    virtual void BumpByGas(Dir dir, bool inside = false) override {};
    virtual bool CheckPassable() override;
    std::string KV_SAVEBLE(type);
    int KV_SAVEBLE(damage);
    virtual void AfterWorldCreation() override;
private:
    Dir KV_SAVEBLE(dMove);
    VDir target_;
    std::vector<VDir> movement_;
    int KV_SAVEBLE(current_step_);
    bool KV_SAVEBLE(reached_target);
    int KV_SAVEBLE(set_target_);
};
ADD_TO_TYPELIST(Projectile);
