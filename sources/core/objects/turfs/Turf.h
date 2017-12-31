#pragma once

#include "objects/MaterialObject.h"
#include "atmos/AtmosHolder.h"

namespace friction
{
    const int BASE_FRICTION = 100;
}

namespace kv
{

class Turf : public MaterialObject
{
public:
    DECLARE_SAVEABLE(Turf, MaterialObject);
    REGISTER_CLASS_AS(Turf);
    Turf();
    virtual void Delete() override;

    int GetFriction() const { return friction_; }
    void SetFriction(int friction) { friction_ = friction; }

    virtual void Represent(GrowingFrame* frame) override;

    atmos::AtmosState GetAtmosState() const { return atmos_state_; }
    void SetAtmosState(atmos::AtmosState atmos_state) { atmos_state_ = atmos_state; }
private:
    int KV_SAVEABLE(friction_);
    atmos::AtmosState KV_SAVEABLE(atmos_state_);
};
END_DECLARE(Turf);

}

namespace friction
{
    int CombinedFriction(IdPtr<kv::Turf> turf);
}
