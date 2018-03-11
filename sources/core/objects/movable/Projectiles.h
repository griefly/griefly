#pragma once

#include "Movable.h"
#include "objects/mobs/Human.h"
#include "SaveableOperators.h"

namespace kv
{

class Projectile : public Movable
{
public:
    DECLARE_SAVEABLE(Projectile, Movable);
    REGISTER_CLASS_AS(Projectile);

    Projectile();

    void MakeMovementPattern(Vector target, Dir facing);
    virtual void Process() override;
    virtual void BumpByGas(const Vector& vector, bool inside = false) override { }
    virtual void AfterWorldCreation() override;
    virtual void ProcessForce() override {}
    void MakeMovementLoops(int d1_number, int d2_number, Dir d1, Dir d2);
    int GetDamage();
    int GetBurnDamage();
    virtual bool CheckPassable() override;
protected:
    bool CheckHumanTile();
    bool KV_SAVEABLE(harmless_);

    int KV_SAVEABLE(damage_);
    int KV_SAVEABLE(burn_damage_);
private:
    bool ProcessMovement();

    int KV_SAVEABLE(current_step_);
    QVector<Dir> KV_SAVEABLE(movement_);
};
END_DECLARE(Projectile);

}
