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

    void SetLying(bool value);
    bool GetLying() const { return lying_; }

    void AddLyingTimer(int value);

    virtual void AttackBy(id_ptr_on<Item> item) override;

    virtual void processImage(DrawType type) override;

    virtual bool checkMove(Dir direct) override;
    virtual InterfaceBase* GetInterface() override { return &interface_; }

    virtual void CalculateVisible(std::list<point>* visible_list) override;

    void UpdateOverlays();

    int GetHealth() { return health_; }
private:
    HumanInterface KV_SAVEBLE(interface_);

    int KV_SAVEBLE(lay_timer_);

    bool KV_SAVEBLE(lying_);
    bool KV_SAVEBLE(dead_);

    int KV_SAVEBLE(health_);
};
ADD_TO_TYPELIST(Human);
