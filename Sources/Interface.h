#pragma once

#include <vector>

#include "Slot.h"

class InterfaceBase
{
public:
    virtual bool Click(int x, int y) = 0;
    virtual void Draw() = 0;
};

class HumanInterface: public InterfaceBase
{
public:
    virtual bool Click(int x, int y) override;
    virtual void Draw() override;
    void InitSlots();
    ~HumanInterface();
private:
    std::vector<BaseSlot*> slots_;
};