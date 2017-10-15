#pragma once

#include "Mob.h"
#include "representation/ViewInfo.h"

namespace kv
{

class LoginMob : public Mob
{
public:
    DECLARE_SAVEABLE(LoginMob, Mob);
    REGISTER_CLASS_AS(LoginMob);
    LoginMob();

    virtual void AfterWorldCreation() override;

    virtual void MindExit() override;
    virtual void MindEnter() override;
    virtual void GenerateInterfaceForFrame(GrowingFrame* representation) override;
    virtual void ProcessMessage(const WorldInterface::Message& message) override;

    virtual Position GetPosition() const override
    {
        return Position();
    }

    virtual void CalculateVisible(VisiblePoints* visible_list) const override;
private:
    ViewInfo KV_SAVEABLE(login_view_);
};
END_DECLARE(LoginMob);

}
