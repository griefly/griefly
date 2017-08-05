#include "HumanInterface.h"

#include "core/objects/movable/items/Item.h"
#include "core/objects/mobs/Human.h"

#include "representation/Representation.h"

namespace
{
    // Slots
    const QString LEFT_HAND = "left_hand";
    const QString RIGHT_HAND = "right_hand";
    const QString HEAD = "head";
    const QString SUIT = "suit";
    const QString UNIFORM = "uniform";
    const QString FEET = "feet";

    // Buttons
    const QString DROP = "drop_active";
    const QString STOP_PULL = "stop_pull";
    const QString SWAP = "swap_hands";
    const QString LAY = "switch_lay";

    // Slots & buttons & indicators sprites
    const QString DEFAULT_INTERFACE_SPRITE = "icons/screen_retro.dmi";
    const QString OLD_INTERFACE_SPRITE = "icons/screen1_old.dmi";

    // Pull indicator states
    const QString NOT_PULL_STATE = "pull0";
    const QString PULL_STATE = "pull1";
}

kv::HumanInterface2::HumanInterface2()
    : active_hand_(true)
{
    {
        Slot right_hand;
        right_hand.position = {0, 14};
        right_hand.view.SetSprite(DEFAULT_INTERFACE_SPRITE);
        right_hand.view.SetState("hand_r_active");
        right_hand.overlay_sprite = "icons/items_righthand.dmi";
        right_hand.name = RIGHT_HAND;
        right_hand.type = SlotType::ANYTHING;
        slots_.append(right_hand);
    }

    {
        Slot left_hand;
        left_hand.position = {2, 14};
        left_hand.view.SetSprite(DEFAULT_INTERFACE_SPRITE);
        left_hand.view.SetState("hand_l_inactive");
        left_hand.overlay_sprite = "icons/items_lefthand.dmi";
        left_hand.name = LEFT_HAND;
        left_hand.type = SlotType::ANYTHING;
        slots_.append(left_hand);
    }

    {
        Slot head;
        head.position = {1, 13};
        head.view.SetSprite(DEFAULT_INTERFACE_SPRITE);
        head.view.SetState("head");
        head.overlay_sprite = "icons/head.dmi";
        head.name = HEAD;
        head.type = SlotType::HEAD;
        slots_.append(head);
    }

    {
        Slot suit;
        suit.position = {1, 14};
        suit.view.SetSprite(DEFAULT_INTERFACE_SPRITE);
        suit.view.SetState("suit");
        suit.overlay_sprite = "icons/suit.dmi";
        suit.type = SlotType::SUIT;
        suit.name = SUIT;
        slots_.append(suit);
    }

    {
        Slot uniform;
        uniform.position = {1, 15};
        uniform.view.SetSprite(DEFAULT_INTERFACE_SPRITE);
        uniform.view.SetState("uniform");
        uniform.overlay_sprite = "icons/uniform.dmi";
        uniform.overlay_state_postfix = "_s";
        uniform.type = SlotType::UNIFORM;
        uniform.name = UNIFORM;
        slots_.append(uniform);
    }

    {
        Slot feet;
        feet.position = {3, 14};
        feet.view.SetSprite(DEFAULT_INTERFACE_SPRITE);
        feet.view.SetState("shoes");
        feet.overlay_sprite = "icons/feet.dmi";
        feet.type = SlotType::FEET;
        feet.name = FEET;
        slots_.append(feet);
    }

    {
        Button pull;
        pull.position = {8, 15};
        pull.view.SetSprite(OLD_INTERFACE_SPRITE);
        pull.view.SetState(NOT_PULL_STATE);
        pull.name = STOP_PULL;
        buttons_.append(pull);
    }

    {
        Button drop;
        drop.position = {7, 15};
        drop.view.SetSprite(OLD_INTERFACE_SPRITE);
        drop.view.SetState("act_drop");
        drop.name = DROP;
        buttons_.append(drop);
    }
}

void kv::HumanInterface2::SetOwner(IdPtr<Human> human)
{
    owner_ = human;
}

void kv::HumanInterface2::HandleClick(const QString& name)
{
    for (Slot& slot : slots_)
    {
        if (slot.name == name)
        {
            ApplyActiveHandOnSlot(&slot);
            return;
        }
    }
    if (name == STOP_PULL)
    {
        owner_->StopPull();
        return;
    }
    else if (name == DROP)
    {
        DropItem();
        return;
    }
    // TODO: other non-item UI elements
}

bool kv::HumanInterface2::PickItem(IdPtr<Item> item)
{
    Slot& active_hand = GetActiveHand();
    return InsertItem(&active_hand, item);
}

void kv::HumanInterface2::DropItem()
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

IdPtr<kv::Item> kv::HumanInterface2::GetItemInActiveHand()
{
    Slot& active_hand = GetActiveHand();
    return active_hand.item;
}

IdPtr<kv::Item> kv::HumanInterface2::GetItem(const QString& slot_name)
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

void kv::HumanInterface2::RemoveItem(const QString& slot_name)
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

void kv::HumanInterface2::RemoveItem(Slot* slot)
{
    slot->item = 0;
}

bool kv::HumanInterface2::InsertItem(const QString& slot_name, IdPtr<Item> item)
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

bool kv::HumanInterface2::InsertItem(Slot* slot, IdPtr<Item> item)
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

void kv::HumanInterface2::Represent()
{
    for (const Slot& slot : slots_)
    {
        Representation::InterfaceUnit unit;
        unit.name = slot.name;
        unit.pixel_x = 32 * slot.position.first;
        unit.pixel_y = 32 * slot.position.second;
        unit.view = slot.view;
        GetRepresentation().AddToNewFrame(unit);
        if (slot.item.IsValid())
        {
            Representation::InterfaceUnit unit;
            unit.name = slot.name;
            unit.pixel_x = 32 * slot.position.first;
            unit.pixel_y = 32 * slot.position.second;
            unit.view = *(slot.item->GetView());
            GetRepresentation().AddToNewFrame(unit);
        }
    }
}

void kv::HumanInterface2::RemoveItem(IdPtr<Item> item)
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

void kv::HumanInterface2::AddOverlays()
{
    for (const Slot& slot : slots_)
    {
        if (slot.item.IsValid())
        {
            const QString state_name = slot.item->GetView()->GetBaseFrameset().GetState();
            owner_->GetView()->AddOverlay(slot.overlay_sprite, state_name + slot.overlay_state_postfix);
        }
    }
}

void kv::HumanInterface2::UpdatePulling(const bool is_pulling)
{
    const QString state = is_pulling ? PULL_STATE : NOT_PULL_STATE;
    Button& pull = GetButton(STOP_PULL);
    pull.view.SetState(state);
}

kv::Slot& kv::HumanInterface2::GetSlot(const QString& slot_name)
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

kv::Slot& kv::HumanInterface2::GetActiveHand()
{
    QString active_hand_name = RIGHT_HAND;
    if (!active_hand_)
    {
        active_hand_name = LEFT_HAND;
    }
    return GetSlot(active_hand_name);
}

void kv::HumanInterface2::ApplyActiveHandOnSlot(Slot* slot)
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

void kv::HumanInterface2::SwapHands()
{
    Slot& right_hand = GetSlot(RIGHT_HAND);
    Slot& left_hand = GetSlot(LEFT_HAND);
    if (active_hand_)
    {
        right_hand.view.SetState("hand_r_inactive");
        left_hand.view.SetState("hand_l_active");
    }
    else
    {
        right_hand.view.SetState("hand_r_active");
        left_hand.view.SetState("hand_l_inactive");
    }
    active_hand_ = !active_hand_;
}

kv::Button& kv::HumanInterface2::GetButton(const QString& button_name)
{
    for (Button& button : buttons_)
    {
        if (button.name == button_name)
        {
            return button;
        }
    }
    KvAbort(QString("No such button in HumanInterface: %1").arg(button_name));
}
