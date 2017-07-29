#include "HumanInterface.h"

#include "core/objects/movable/items/Item.h"
#include "core/objects/mobs/Human.h"

#include "representation/Representation.h"

using namespace kv;

namespace
{
    const QString LEFT_HAND = "left_hand";
    const QString RIGHT_HAND = "right_hand";
}

HumanInterface2::HumanInterface2()
    : active_hand_(true)
{
    // Nothing
}

void HumanInterface2::SetOwner(IdPtr<Human> human)
{
    owner_ = human;
}

void HumanInterface2::HandleClick(const QString& name)
{
    for (Slot& slot : slots_)
    {
        if (slot.name == name)
        {
            ApplyActiveHandOnSlot(&slot);
            return;
        }
    }
    // TODO: non-item UI elements
}

bool HumanInterface2::PickItem(IdPtr<Item> item)
{
    Slot& active_hand = GetActiveHand();
    return InsertItem(&active_hand, item);
}

void HumanInterface2::DropItem()
{
    Slot& active_hand = GetActiveHand();
    if (active_hand.item)
    {
        // TODO: what if AddObject somehow will force to PickItem or DropItem
        // again?
        if (owner_->GetOwner()->AddObject(active_hand.item))
        {
            RemoveItem(&active_hand);
        }
    }
}

IdPtr<Item> HumanInterface2::GetItemInActiveHand()
{
    Slot& active_hand = GetActiveHand();
    return active_hand.item;
}

IdPtr<Item> HumanInterface2::GetItem(const QString& slot_name)
{
    for (Slot& slot : slots_)
    {
        if (slot.name == slot_name)
        {
            return slot.item;
        }
    }
    return 0;
}

void HumanInterface2::RemoveItem(const QString& slot_name)
{
    for (Slot& slot : slots_)
    {
        if (slot.name == slot_name)
        {
            RemoveItem(&slot);
            return;
        }
    }
}

void HumanInterface2::RemoveItem(Slot* slot)
{
    slot->item = 0;
}

bool HumanInterface2::InsertItem(const QString& slot_name, IdPtr<Item> item)
{
    for (Slot& slot : slots_)
    {
        if (slot.name == slot_name)
        {
            return InsertItem(&slot, item);
        }
    }
    return false;
}

bool HumanInterface2::InsertItem(Slot* slot, IdPtr<Item> item)
{
    if (slot->item.IsValid())
    {
        return false;
    }
    if (!IsTypeMatch(*slot, item->type))
    {
        return false;
    }
    slot->item = item;
    item->SetOwner(owner_);
    return true;
}

void HumanInterface2::Represent()
{
    for (const Slot& slot : slots_)
    {
        Representation::InterfaceUnit unit;
        unit.name = slot.name;
        unit.pixel_x = 32 * slot.posx;
        unit.pixel_y = 32 * slot.posy;
        unit.shift = 0;
        unit.view = slot.view;
        GetRepresentation().AddToNewFrame(unit);
        if (slot.item.IsValid())
        {
            Representation::InterfaceUnit unit;
            unit.name = slot.name;
            unit.pixel_x = 32 * slot.posx;
            unit.pixel_y = 32 * slot.posy;
            unit.shift = 0;
            unit.view = *(slot.item->GetView());
            GetRepresentation().AddToNewFrame(unit);
        }
    }
    // TODO: non-item UI elements
}

void HumanInterface2::RemoveItem(IdPtr<Item> item)
{
    for (Slot& slot : slots_)
    {
        if (slot.item == item)
        {
            slot.item = 0;
            return;
        }
    }
}

kv::Slot& HumanInterface2::GetSlot(const QString& slot_name)
{
    for (Slot& slot : slots_)
    {
        if (slot.name == slot_name)
        {
            return slot;
        }
    }
    KvAbort(QString("No such slot in HumanInterface: %1").arg(slot_name));
}

kv::Slot& HumanInterface2::GetActiveHand()
{
    QString active_hand_name = RIGHT_HAND;
    if (!active_hand_)
    {
        active_hand_name = LEFT_HAND;
    }
    return GetSlot(active_hand_name);
}

void HumanInterface2::ApplyActiveHandOnSlot(Slot* slot)
{
    Slot& active_hand = GetActiveHand();
    if (active_hand.item.IsValid() && !slot->item.IsValid())
    {
        if (!IsTypeMatch(*slot, active_hand.item->type))
        {
            return;
        }
        slot->item = active_hand.item;
        active_hand.item = 0;
    }
    else if (!active_hand.item.IsValid() && slot->item.IsValid())
    {
        if (!IsTypeMatch(active_hand, slot->item->type))
        {
            return;
        }
        active_hand.item = slot->item;
        slot->item = 0;
    }
    else if (slot->item.IsValid())
    {
        slot->item->AttackBy(active_hand.item);
    }
}
