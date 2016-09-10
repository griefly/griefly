#pragma once

#include <vector>

#include "Slot.h"
#include "Item.h"

class InterfaceBase
{
public:
    virtual void HandleClick(const std::string& place) = 0;
    virtual void Draw() = 0;
    virtual unsigned int hash() const = 0;
    virtual ~InterfaceBase() {}
};

namespace HumanInterfacePlaces
{
    const std::string UNCLICKABLE = "UNCLICKABLE";
    const std::string RIGHT_HAND = "INT_RHAND";
    const std::string LEFT_HAND = "INT_LHAND";
    const std::string HEAD = "HEAD";
    const std::string SUIT = "SUIT";
    const std::string UNIFORM = "UNIFORM";
    const std::string FEET = "FEET";
    const std::string DROP = "DROP";
    const std::string STOP_PULL = "STOP_PULL";
    const std::string SWAP = "SWAP";
    const std::string LAY = "SWITCH_LAY";
}

class HumanInterface: public InterfaceBase
{
public:
    void SetOwner(IdPtr<IOnMapObject> owner)
    {
        owner_ = owner;
    }

    virtual void HandleClick(const std::string& place) override;
    virtual void Draw() override;
    void InitSlots();
    virtual ~HumanInterface();
    friend std::ostream& operator<<(std::stringstream& file, HumanInterface& interf);
    friend std::istream& operator>>(std::stringstream& file, HumanInterface& interf);
    virtual unsigned int hash() const override;

    void Pick(IdPtr<Item> item);
    void Drop();

    void AddOverlays();

    IdPtr<Item> GetRHand();
    IdPtr<Item> GetLHand();
    Slot<Item>& GetActiveHand();
    void SwapHands();

    void UpdateHealth();
    void UpdateLaying();
    void UpdatePulling(bool isPulling);
    void StopPulling();

public:
    void ApplyActiveHandOnSlot(Slot<Item>* slot);

    IdPtr<IOnMapObject> owner_;

    bool active_hand_;
    bool pulling_;

    Slot<Item> drop_;
    Slot<Item> swap_;
    Slot<Item> stop_pull_;

    Slot<Item> health_;

    Slot<Item> lay_;

    Slot<Item> r_hand_;
    Slot<Item> l_hand_;
    Slot<Item> head_;
    Slot<Item> suit_;
    Slot<Item> uniform_;
    Slot<Item> feet_;
};

std::ostream& operator<<(std::stringstream& file, HumanInterface& interf);
std::istream& operator>>(std::stringstream& file, HumanInterface& interf);

inline unsigned int hash(const HumanInterface& i)
{
    return i.hash();
}
