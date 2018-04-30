#pragma once

#include "objects/MaterialObject.h"

namespace kv
{

class Movable : public MaterialObject
{
public:
    DECLARE_SAVEABLE(Movable, MaterialObject);
    REGISTER_CLASS_AS(Movable);
    Movable();
    virtual bool TryMove(Dir direct);
    bool CheckMoveTime();
    virtual bool CheckPassable();
    bool MainMove();

    virtual bool Rotate(Dir dir);
    Dir GetDir() const { return direction_; }

    virtual void ProcessForce();

    virtual void ApplyForce(const Vector& force, ForceSource source = ForceSource::UNKNOWN) override;

    virtual void Delete() override { MaterialObject::Delete(); }

    virtual void Represent(GrowingFrame* frame, IdPtr<Mob> mob) const override;
    virtual void Bump(const Vector& force, IdPtr<Movable> item) override;
    virtual void BumpByGas(const Vector& force, bool inside = false) override;

    const Vector& GetForce() const { return force_; }

    bool IsAnchored() const { return anchored_; }
    void SetAnchored(bool anchored) { anchored_ = anchored; }

    qint32 GetTickSpeed() const { return tick_speed_; }
    void SetTickSpeed(qint32 tick_speed) { tick_speed_ = tick_speed; }

private:
    bool KV_SAVEABLE(anchored_);
    qint32 KV_SAVEABLE(last_move_);
    qint32 KV_SAVEABLE(tick_speed_);
    Dir KV_SAVEABLE(direction_);

    Vector KV_SAVEABLE(force_);
    Dir KV_SAVEABLE(main_force_direction_);
    Dir KV_SAVEABLE(secondary_force_direction_);
    qint32 KV_SAVEABLE(force_error_);
    qint32 KV_SAVEABLE(force_error_per_main_);
};
END_DECLARE(Movable);

}
