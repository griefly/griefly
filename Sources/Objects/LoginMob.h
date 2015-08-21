#pragma once

#include "MobInt.h"

class LoginMob: public IMob
{
public:
    DECLARE_SAVED(LoginMob, IMob);
    DECLARE_GET_TYPE_ITEM(LoginMob);
    LoginMob(size_t id);
    virtual void DeinitGUI() override;
    virtual void InitGUI() override;
    virtual void processGUI() override;//draw
    virtual void processGUImsg(const Message& msg) override;

    virtual void process() override;

    virtual int GetDrawX() const override
    {
        return 0;
    }

    virtual int GetDrawY() const override
    {
        return 0;
    }

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

    // virtual InterfaceBase* GetInterface() override { return &interface_; }

    virtual void CalculateVisible(std::list<point>* visible_list) override;
};
ADD_TO_TYPELIST(LoginMob);
