#pragma once

#include <vector>

#include "Slot.h"
#include "Item.h"

class InterfaceBase
{
public:
    virtual bool Click(int x, int y) = 0;
    virtual void HandleClick(const std::string& place) = 0;
    virtual void Draw() = 0;
    virtual unsigned int hash() const = 0;
    virtual ~InterfaceBase() {}
};

class HumanInterface: public InterfaceBase
{
public:
    void SetOwner(id_ptr_on<IOnMapObject> owner)
    {
        owner_ = owner;
    }

    virtual bool Click(int x, int y) override;
    virtual void HandleClick(const std::string& place) override;
    virtual void Draw() override;
    void InitSlots();
    virtual ~HumanInterface();
    friend std::ostream& operator<<(std::stringstream& file, HumanInterface& interf);
    friend std::istream& operator>>(std::stringstream& file, HumanInterface& interf);
    virtual unsigned int hash() const override;

    void Pick(id_ptr_on<Item> item);
    void Drop();

    void AddOverlays();

    id_ptr_on<Item> GetRHand();
    id_ptr_on<Item> GetLHand();
    Slot<Item>& GetActiveHand();
    void SwapHands();
private:
    void ApplyActiveHandOnSlot(Slot<Item>* slot);

    id_ptr_on<IOnMapObject> owner_;

    bool active_hand_;

    Slot<Item> drop_;
    Slot<Item> swap_;

    Slot<Item> r_hand_;
    Slot<Item> l_hand_;
    Slot<Item> head_;
    Slot<Item> suit_;
};

std::ostream& operator<<(std::stringstream& file, HumanInterface& interf);
std::istream& operator>>(std::stringstream& file, HumanInterface& interf);

inline unsigned int hash(const HumanInterface& i)
{
    return i.hash();
}
