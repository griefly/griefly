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
    virtual void Represent(Representation* representation) override;
    virtual void CalculateVisible(VisiblePoints* visible_list) const override;
    virtual void ProcessMessage(const Message& msg) override;

    virtual void GenerateInterfaceForFrame(Representation* representation) override;

    virtual void MindEnter() override;
    virtual void MindExit() override;

    virtual void Process() override;
private:
    int KV_SAVEABLE(seconds_until_respawn_);
    bool IsMobGhost();
};
END_DECLARE(Ghost);

}
