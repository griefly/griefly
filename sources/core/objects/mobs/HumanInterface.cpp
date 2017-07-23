#include "HumanInterface.h"

#include "core/objects/movable/items/Item.h"
#include "representation/Representation.h"

using namespace kv;

HumanInterface2::HumanInterface2()
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
            slot.item = 0;
            return;
        }
    }
}

bool HumanInterface2::InsertItem(const QString& slot_name, IdPtr<Item> item)
{
    for (Slot& slot : slots_)
    {
        if (slot.name == slot_name)
        {
            // TODO: change item->type type from QString to SlotType
            // if (slot.type != item->type)
            // {
            //     return false;
            // }
            slot.item = item;
            item->SetOwner(owner_);
        }
    }
    return false;
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

void HumanInterface2::ApplyActiveHandOnSlot(Slot* slot)
{
    // TODO
}
