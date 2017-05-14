#pragma once

#include "Mob.h"

namespace kv
{

class Ghost: public IMob
{
public:
    DECLARE_SAVEABLE(Ghost, IMob);
    REGISTER_CLASS_AS(Ghost);
    Ghost();
    virtual void AfterWorldCreation() override;
    virtual void Represent() override;
    virtual void CalculateVisible(std::list<PosPoint>* visible_list) override;
    virtual void ProcessMessage(const Message2& msg) override;

    virtual void InitGUI() override;
    virtual void DeinitGUI() override;

    virtual void Process() override;
private:
    int KV_SAVEABLE(seconds_until_respawn_);
    bool IsMobGhost();
};
END_DECLARE(Ghost);

}
