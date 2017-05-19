#pragma once

#include "Mob.h"
#include "Interface.h"
#include "representation/ViewInfo.h"

class LoginInterface: public InterfaceBase
{
    friend FastSerializer& operator<<(FastSerializer& file, LoginInterface& interf);
    friend FastDeserializer& operator>>(FastDeserializer& file, LoginInterface& interf);
public:
    void Init();

    virtual void HandleClick(const QString& place) override {}
    virtual void Draw() override;
    virtual unsigned int hash() const override;
private:
    ViewInfo view_;
};

FastSerializer& operator<<(FastSerializer& file, LoginInterface& interf);
FastDeserializer& operator>>(FastDeserializer& file, LoginInterface& interf);

inline unsigned int hash(const LoginInterface& i)
{
    return i.hash();
}

namespace kv
{

class LoginMob : public Mob
{
public:
    DECLARE_SAVEABLE(LoginMob, Mob);
    REGISTER_CLASS_AS(LoginMob);
    LoginMob();

    virtual void AfterWorldCreation() override;

    virtual void DeinitGUI() override;
    virtual void InitGUI() override;
    virtual void GenerateInterfaceForFrame() override;
    virtual void ProcessMessage(const Message2& msg) override;

    virtual void Process() override;

    virtual int GetX() const override
    {
        return 0;
    }

    virtual int GetY() const override
    {
        return 0;
    }

    virtual int GetZ() const override
    {
        return 0;
    }

    // virtual void processImage(DrawType type) override;

    virtual InterfaceBase* GetInterface() override { return &interface_; }

    virtual void CalculateVisible(std::list<PosPoint>* visible_list) override;
private:
    LoginInterface KV_SAVEABLE(interface_);
};
END_DECLARE(LoginMob);

}
