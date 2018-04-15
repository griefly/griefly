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
    virtual void BumpByGas(const Vector& /*vector*/, bool /*inside = false*/) override { }
    virtual void AfterWorldCreation() override;
    virtual void ProcessForce() override {}
    void MakeMovementLoops(int d1_number, int d2_number, Dir d1, Dir d2);

    qint32 GetDamage() const { return damage_; }
    void SetDamage(qint32 damage) { damage_ = damage; }

    qint32 GetBurnDamage() const { return burn_damage_; }
    void SetBurnDamage(qint32 damage) { burn_damage_ = damage; }

    virtual bool CheckPassable() override;
protected:
    bool CheckHumanTile();
    bool KV_SAVEABLE(harmless_);
private:
    bool ProcessMovement();

    qint32 KV_SAVEABLE(damage_);
    qint32 KV_SAVEABLE(burn_damage_);

    qint32 KV_SAVEABLE(current_step_);
    QVector<Dir> KV_SAVEABLE(movement_);
};
END_DECLARE(Projectile);

}
