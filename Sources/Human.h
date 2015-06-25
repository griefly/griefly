#pragma once

#include "MobInt.h"

class Human: public IMob
{
public:
    DECLARE_GET_TYPE_ITEM(Human);
    DECLARE_SAVED(Human, IMob);
    Human(size_t id);
    virtual void DeinitGUI() override;
    virtual void InitGUI() override;
    virtual void processGUI() override;//draw
    virtual void processGUImsg(const Message& msg) override;
    virtual void process() override;
    virtual void Live();

    virtual void processImage(DrawType type) override;

    virtual bool checkMove(Dir direct) override;
    virtual InterfaceBase* GetInterface() override { return &interface_; }

    virtual void CalculateVisible(std::list<point>* visible_list) override;

    void UpdateOverlays();
private:
    HumanInterface KV_SAVEBLE(interface_);

    bool KV_SAVEBLE(lying_);
    bool KV_SAVEBLE(dead_);

    int KV_SAVEBLE(health_);
};
ADD_TO_TYPELIST(Human);
