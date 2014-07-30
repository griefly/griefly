#pragma once

#include <vector>

#include "Slot.h"

class InterfaceBase
{
public:
    virtual bool Click(int x, int y) = 0;
    virtual void Draw() = 0;
    virtual unsigned int hash() const = 0;
    virtual ~InterfaceBase() {};
};

class HumanInterface: public InterfaceBase
{
public:
    virtual bool Click(int x, int y) override;
    virtual void Draw() override;
    void InitSlots();
    virtual ~HumanInterface();
    friend std::ostream& operator<<(std::stringstream& file, HumanInterface& interf);
    friend std::istream& operator>>(std::stringstream& file, HumanInterface& interf);
    virtual unsigned int hash() const override;
private:
    Slot<Item> r_hand_;
};

std::ostream& operator<<(std::stringstream& file, HumanInterface& interf);
std::istream& operator>>(std::stringstream& file, HumanInterface& interf);

inline unsigned int hash(const HumanInterface& i)
{
    return i.hash();
}