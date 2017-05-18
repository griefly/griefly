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

namespace kv
{
    class Human;
}

class HumanInterface : public InterfaceBase
{
public:
    void SetOwner(IdPtr<kv::MaterialObject> owner)
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

    void Pick(IdPtr<kv::Item> item);
    void Drop();

    void AddOverlays();

    IdPtr<kv::Item> GetRHand();
    IdPtr<kv::Item> GetLHand();
    Slot<kv::Item>& GetActiveHand();
    void SwapHands();

    void UpdateHealth();
    void UpdateLaying();
    void UpdatePulling(bool is_pulling);
    void UpdateEnvironment();

public:
    void ApplyActiveHandOnSlot(Slot<kv::Item>* slot);

    IdPtr<kv::Human> owner_;

    bool active_hand_;
    bool pulling_;

    Slot<kv::Item> drop_;
    Slot<kv::Item> swap_;
    Slot<kv::Item> stop_pull_;

    Slot<kv::Item> health_;
    Slot<kv::Item> temperature_;
    Slot<kv::Item> oxygen_;

    Slot<kv::Item> lay_;

    Slot<kv::Item> r_hand_;
    Slot<kv::Item> l_hand_;
    Slot<kv::Item> head_;
    Slot<kv::Item> suit_;
    Slot<kv::Item> uniform_;
    Slot<kv::Item> feet_;
private:
    void StopPull();
};

FastSerializer& operator<<(FastSerializer& file, HumanInterface& interf);
FastDeserializer& operator>>(FastDeserializer& file, HumanInterface& interf);

inline unsigned int hash(const HumanInterface& i)
{
    return i.hash();
}
