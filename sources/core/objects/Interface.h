#pragma once

#include <vector>

#include "Slot.h"
#include "Item.h"

class InterfaceBase
{
public:
    virtual void HandleClick(const QString& place) = 0;
    virtual void Draw() = 0;
    virtual unsigned int hash() const = 0;
    virtual ~InterfaceBase() {}
};

namespace HumanInterfacePlaces
{
    const QString UNCLICKABLE = "UNCLICKABLE";
    const QString RIGHT_HAND = "INT_RHAND";
    const QString LEFT_HAND = "INT_LHAND";
    const QString HEAD = "HEAD";
    const QString SUIT = "SUIT";
    const QString UNIFORM = "UNIFORM";
    const QString FEET = "FEET";
    const QString DROP = "DROP";
    const QString STOP_PULL = "STOP_PULL";
    const QString SWAP = "SWAP";
    const QString LAY = "SWITCH_LAY";
}

class Human;

class HumanInterface : public InterfaceBase
{
public:
    void SetOwner(IdPtr<kv::IOnMapObject> owner)
    {
        owner_ = owner;
    }

    virtual void HandleClick(const QString& place) override;
    virtual void Draw() override;
    void InitSlots();
    virtual ~HumanInterface();
    friend FastSerializer& operator<<(FastSerializer& file, HumanInterface& interf);
    friend FastDeserializer& operator>>(FastDeserializer& file, HumanInterface& interf);
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
    void UpdatePulling(bool is_pulling);
    void UpdateEnvironment();

public:
    void ApplyActiveHandOnSlot(Slot<Item>* slot);

    IdPtr<Human> owner_;

    bool active_hand_;
    bool pulling_;

    Slot<Item> drop_;
    Slot<Item> swap_;
    Slot<Item> stop_pull_;

    Slot<Item> health_;
    Slot<Item> temperature_;
    Slot<Item> oxygen_;

    Slot<Item> lay_;

    Slot<Item> r_hand_;
    Slot<Item> l_hand_;
    Slot<Item> head_;
    Slot<Item> suit_;
    Slot<Item> uniform_;
    Slot<Item> feet_;
private:
    void StopPull();
};

FastSerializer& operator<<(FastSerializer& file, HumanInterface& interf);
FastDeserializer& operator>>(FastDeserializer& file, HumanInterface& interf);

inline unsigned int hash(const HumanInterface& i)
{
    return i.hash();
}
