#pragma once

#include "MobInt.h"

class Human: public IMob
{
public:
    DECLARE_GET_TYPE_ITEM(Human);
    DECLARE_SAVED(Human, IMob);
    Human();
    virtual void DeinitGUI() override;
    virtual void InitGUI() override;
    virtual void processGUI() override;//draw
    virtual void processGUImsg(const Message& msg) override;
    virtual void process() override;
    virtual void Live();

    virtual bool checkMove(Dir direct) override;
    virtual InterfaceBase* GetInterface() override { return &interface_; }
private:
    HumanInterface KV_SAVEBLE(interface_);

    bool KV_SAVEBLE(lying_);

    int KV_SAVEBLE(health_);
};
ADD_TO_TYPELIST(Human);