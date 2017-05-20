#pragma once

#include "Mob.h"

namespace kv
{

class Ghost : public Mob
{
public:
    DECLARE_SAVEABLE(Ghost, Mob);
    REGISTER_CLASS_AS(Ghost);
    Ghost();
    virtual void AfterWorldCreation() override;
    virtual void Represent() override;
    virtual void CalculateVisible(std::list<PosPoint>* visible_list) override;
    virtual void ProcessMessage(const Message2& msg) override;

    virtual void InitGui() override;
    virtual void DeinitGui() override;

    virtual void Process() override;
private:
    int KV_SAVEABLE(seconds_until_respawn_);
    bool IsMobGhost();
};
END_DECLARE(Ghost);

}
