#include "HumanInterface.h"

#include "objects/movable/items/Item.h"
#include "objects/mobs/Human.h"

namespace
{
    // Buttons
    const QString DROP = "drop_active";
    const QString STOP_PULL = "stop_pull";
    const QString SWAP = "swap_hands";
    const QString LAY = "switch_lay";
    const QString HEALTH = "health";
    const QString OXYGEN = "oxygen";
    const QString TOXINS = "toxins";
    const QString TEMPERATURE = "temperature";

    // Slots & buttons & indicators sprites
    const QString DEFAULT_INTERFACE_SPRITE = "icons/screen_retro.dmi";
    const QString OLD_INTERFACE_SPRITE = "icons/screen1_old.dmi";

    namespace states
    {
        // Hands states
        const QString RIGHT_HAND_INACTIVE = "hand_r_inactive";
        const QString RIGHT_HAND_ACTIVE = "hand_r_active";
        const QString LEFT_HAND_INACTIVE = "hand_l_inactive";
        const QString LEFT_HAND_ACTIVE = "hand_l_active";

        // Swap indicator states
        const QString RIGHT_HAND = "hand_right";
        const QString LEFT_HAND = "hand_left";

        // Pull indicator states
        const QString NOT_PULL = "pull0";
        const QString PULL = "pull1";

        // Lay indicator states
        const QString NOT_LAY = "rest0";
        const QString LAY = "rest1";

        // Oxygen states
        const QString OXYGEN = "oxy0";
        const QString NO_OXYGEN = "oxy1";

        // Toxins states
        const QString TOXINS = "tox1";
        const QString NO_TOXINS = "tox0";

        // Temperature template
        const QString TEMPERATURE_TEMPLATE = "temp%1";
    }
}

kv::HumanInterface::HumanInterface()
    : active_hand_(true)
{
    {
        Slot right_hand;
        right_hand.position = {0, 14};
        right_hand.view.SetSprite(DEFAULT_INTERFACE_SPRITE);
        right_hand.view.SetState(states::RIGHT_HAND_ACTIVE);
        right_hand.overlay_sprite = "icons/items_righthand.dmi";
        right_hand.name = slot::RIGHT_HAND;
        right_hand.type = SlotType::ANYTHING;
        slots_.append(right_hand);
    }

    {
        Slot left_hand;
        left_hand.position = {2, 14};
        left_hand.view.SetSprite(DEFAULT_INTERFACE_SPRITE);
        left_hand.view.SetState(states::LEFT_HAND_INACTIVE);
        left_hand.overlay_sprite = "icons/items_lefthand.dmi";
        left_hand.name = slot::LEFT_HAND;
        left_hand.type = SlotType::ANYTHING;
        slots_.append(left_hand);
    }

    {
        Slot head;
        head.position = {1, 13};
        head.view.SetSprite(DEFAULT_INTERFACE_SPRITE);
        head.view.SetState("head");
        head.overlay_sprite = "icons/head.dmi";
        head.name = slot::HEAD;
        head.type = SlotType::HEAD;
        slots_.append(head);
    }

    {
        Slot mask;
        mask.position = {0, 13};
        mask.view.SetSprite(DEFAULT_INTERFACE_SPRITE);
        mask.view.SetState("mask");
        mask.overlay_sprite = "icons/mask.dmi";
        mask.name = slot::HEAD;
        mask.type = SlotType::HEAD;
        slots_.append(mask);
    }

    {
        Slot uniform;
        uniform.position = {1, 15};
        uniform.view.SetSprite(DEFAULT_INTERFACE_SPRITE);
        uniform.view.SetState("uniform");
        uniform.overlay_sprite = "icons/uniform.dmi";
        uniform.overlay_state_postfix = "_s";
        uniform.type = SlotType::UNIFORM;
        uniform.name = slot::UNIFORM;
        slots_.append(uniform);
    }

    {
        Slot feet;
        feet.position = {3, 14};
        feet.view.SetSprite(DEFAULT_INTERFACE_SPRITE);
        feet.view.SetState("shoes");
        feet.overlay_sprite = "icons/feet.dmi";
        feet.type = SlotType::FEET;
        feet.name = slot::FEET;
        slots_.append(feet);
    }

    {
        Slot suit;
        suit.position = {1, 14};
        suit.view.SetSprite(DEFAULT_INTERFACE_SPRITE);
        suit.view.SetState("suit");
        suit.overlay_sprite = "icons/suit.dmi";
        suit.type = SlotType::SUIT;
        suit.name = slot::SUIT;
        slots_.append(suit);
    }

    {
        Button drop;
        drop.position = {7, 15};
        drop.view.SetSprite(OLD_INTERFACE_SPRITE);
        drop.view.SetState("act_drop");
        drop.name = DROP;
        buttons_.append(drop);
    }

    {
        Button pull;
        pull.position = {8, 15};
        pull.view.SetSprite(OLD_INTERFACE_SPRITE);
        pull.view.SetState(states::NOT_PULL);
        pull.name = STOP_PULL;
        buttons_.append(pull);
    }

    {
        Button swap;
        swap.position = {6, 15};
        swap.view.SetSprite(OLD_INTERFACE_SPRITE);
        swap.view.SetState(states::RIGHT_HAND);
        swap.name = SWAP;
        buttons_.append(swap);
    }

    {
        Button lay;
        lay.position = {15, 11};
        lay.view.SetSprite(OLD_INTERFACE_SPRITE);
        lay.view.SetState(states::NOT_LAY);
        lay.name = LAY;
        buttons_.append(lay);
    }

    {
        Button health;
        health.position = {15, 10};
        health.view.SetSprite(OLD_INTERFACE_SPRITE);
        health.view.SetState("health0");
        health.name = HEALTH;
        buttons_.append(health);
    }

    {
        Button oxygen;
        oxygen.position = {15, 12};
        oxygen.view.SetSprite(OLD_INTERFACE_SPRITE);
        oxygen.view.SetState(states::OXYGEN);
        oxygen.name = OXYGEN;
        buttons_.append(oxygen);
    }

    {
        Button toxins;
        toxins.position = {15, 13};
        toxins.view.SetSprite(OLD_INTERFACE_SPRITE);
        toxins.view.SetState(states::TOXINS);
        toxins.name = TOXINS;
        buttons_.append(toxins);
    }

    {
        Button temperature;
        temperature.position = {15, 9};
        temperature.view.SetSprite(OLD_INTERFACE_SPRITE);
        temperature.view.SetState(states::TEMPERATURE_TEMPLATE.arg(0));
        temperature.name = TEMPERATURE;
        buttons_.append(temperature);
    }
}

void kv::HumanInterface::SetOwner(IdPtr<Human> human)
{
    human_owner_ = human;
}

void kv::HumanInterface::HandleClick(const QString& name)
{
    for (Slot& slot : slots_)
    {
        if (slot.name == name)
        {
            ApplyActiveHandOnSlot(&slot);
            human_owner_->UpdateOverlays();
            return;
        }
    }
    if (name == STOP_PULL)
    {
        human_owner_->StopPull();
    }
    else if (name == DROP)
    {
        if (IdPtr<Item> item = GetItemInActiveHand())
        {
            if (human_owner_->GetOwner()->AddObject(item))
            {
                DropItem();
            }
        }
    }
    else if (name == SWAP)
    {
        SwapHands();
    }
    else if (name == LAY)
    {
        bool laying = human_owner_->GetLying();
        if (laying)
        {
            human_owner_->SetLaying(false);
        }
        else
        {
            human_owner_->AddLayingTimer(50);
            human_owner_->SetLaying(true);
        }
    }

    human_owner_->UpdateOverlays();
}

bool kv::HumanInterface::PickItem(IdPtr<Item> item)
{
    Slot& active_hand = GetActiveHand();
    return InsertItem(&active_hand, item);
}

void kv::HumanInterface::DropItem()
{
    Slot& active_hand = GetActiveHand();
    RemoveItem(&active_hand);
}

IdPtr<kv::Item> kv::HumanInterface::GetItemInActiveHand()
{
    const Slot& active_hand = GetActiveHand();
    return active_hand.item;
}

IdPtr<kv::Item> kv::HumanInterface::GetItem(const QString& slot_name) const
{
    for (const Slot& slot : slots_)
    {
        if (slot.name == slot_name)
        {
            return slot.item;
        }
    }
    return 0;
}

void kv::HumanInterface::RemoveItem(const QString& slot_name)
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

void kv::HumanInterface::RemoveItem(Slot* slot)
{
    slot->item = 0;
}

bool kv::HumanInterface::InsertItem(const QString& slot_name, IdPtr<Item> item)
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

bool kv::HumanInterface::InsertItem(Slot* slot, IdPtr<Item> item)
{
    if (slot->item.IsValid())
    {
        return false;
    }
    if (!item.IsValid())
    {
        return false;
    }
    if (!IsTypeMatch(*slot, item->GetItemType()))
    {
        return false;
    }
    slot->item = item;
    item->SetOwner(human_owner_);
    human_owner_->UpdateOverlays();
    return true;
}

void kv::HumanInterface::Represent(GrowingFrame* frame)
{
    for (const Button& button : buttons_)
    {
        FrameData::InterfaceUnit unit;
        unit.name = button.name;
        unit.pixel_x = 32 * button.position.first;
        unit.pixel_y = 32 * button.position.second;
        unit.view = button.view.GetRawData();
        frame->Append(unit);
    }
    for (const Slot& slot : slots_)
    {
        FrameData::InterfaceUnit unit;
        unit.name = slot.name;
        unit.pixel_x = 32 * slot.position.first;
        unit.pixel_y = 32 * slot.position.second;
        unit.view = slot.view.GetRawData();
        frame->Append(unit);
        if (slot.item.IsValid())
        {
            FrameData::InterfaceUnit unit;
            unit.name = slot.name;
            unit.pixel_x = 32 * slot.position.first;
            unit.pixel_y = 32 * slot.position.second;
            unit.view = slot.item->GetView().GetRawData();
            frame->Append(unit);
        }
    }
}

bool kv::HumanInterface::RemoveItem(IdPtr<Item> item)
{
    for (Slot& slot : slots_)
    {
        if (slot.item == item)
        {
            slot.item = 0;
            return true;
        }
    }
    return false;
}

void kv::HumanInterface::AddOverlays(Dir dir, ViewInfo* view_info)
{
    auto add_lay = [view_info, this](auto adder, const QString& slot_name)
    {
        const auto& slot = GetSlot(slot_name);
        if (!slot.item.IsValid())
        {
            return;
        }
        const QString state_name = slot.item->GetView().GetBaseFrameset().GetState();
        (view_info->*adder)(slot.overlay_sprite, state_name + slot.overlay_state_postfix);
    };
    auto add_overlay = [&add_lay](const QString& slot_name)
    {
        add_lay(&ViewInfo::AddOverlay, slot_name);
    };
    auto add_underlay = [&add_lay](const QString& slot_name)
    {
        add_lay(&ViewInfo::AddUnderlay, slot_name);
    };

    if (dir == Dir::WEST)
    {
        add_underlay(slot::RIGHT_HAND);
    }
    else if (dir == Dir::EAST)
    {
        add_underlay(slot::LEFT_HAND);
    }

    add_overlay(slot::UNIFORM);
    add_overlay(slot::FEET);
    add_overlay(slot::HEAD);
    add_overlay(slot::SUIT);

    if (dir == Dir::WEST)
    {
        add_overlay(slot::LEFT_HAND);
    }
    else if (dir == Dir::EAST)
    {
        add_overlay(slot::RIGHT_HAND);
    }
    else
    {
        add_overlay(slot::LEFT_HAND);
        add_overlay(slot::RIGHT_HAND);
    }
}

void kv::HumanInterface::UpdateEnvironment(
    const int temperature, const int pressure, const int oxygen, const int plasma)
{
    Button& oxygen_indicator = GetButton(OXYGEN);

    oxygen_indicator.view.SetState(states::OXYGEN);
    if (oxygen < 1)
    {
        oxygen_indicator.view.SetState(states::NO_OXYGEN);
    }
    int state = qMax(-4, (temperature - REGULAR_TEMPERATURE) / 6);
    state = qMin(4, state);

    if (pressure < ZERO_TEMPERATURE_PRESSURE_BORDER)
    {
        state = -4;
    }

    Button& temperature_indicator = GetButton(TEMPERATURE);
    temperature_indicator.view.SetState(states::TEMPERATURE_TEMPLATE.arg(state));

    Button& toxins_indicator = GetButton(TOXINS);
    toxins_indicator.view.SetState(states::NO_TOXINS);
    if (plasma > HUMAN_PLASMA_TOXINS_THRESHOLD)
    {
        toxins_indicator.view.SetState(states::TOXINS);
    }
}

void kv::HumanInterface::UpdateLaying(const bool is_laying)
{
    Button& lay = GetButton(LAY);
    if (is_laying)
    {
        lay.view.SetState(states::LAY);
    }
    else
    {
        lay.view.SetState(states::NOT_LAY);
    }
}

void kv::HumanInterface::UpdateHealth(const int health)
{
    const int MAX_NON_CRIT_STATE = 5;
    int state = qMax(0, (health * MAX_NON_CRIT_STATE) / HUMAN_MAX_HEALTH); // it will be from 0 to 5
    state = MAX_NON_CRIT_STATE - state; // 0 to 5, but reverted
    if (health < 0)
    {
        state = MAX_NON_CRIT_STATE + 1;
    }
    Button& health_indicator = GetButton(HEALTH);
    health_indicator.view.SetState(QString("health%1").arg(state));
}

void kv::HumanInterface::UpdatePulling(const bool is_pulling)
{
    const QString state = is_pulling ? states::PULL : states::NOT_PULL;
    Button& pull = GetButton(STOP_PULL);
    pull.view.SetState(state);
}

kv::Slot& kv::HumanInterface::GetSlot(const QString& slot_name)
{
    for (Slot& slot : slots_)
    {
        if (slot.name == slot_name)
        {
            return slot;
        }
    }
    kv::Abort(QString("No such slot in HumanInterface: %1").arg(slot_name));
}

kv::Slot& kv::HumanInterface::GetActiveHand()
{
    QString active_hand_name = slot::RIGHT_HAND;
    if (!active_hand_)
    {
        active_hand_name = slot::LEFT_HAND;
    }
    return GetSlot(active_hand_name);
}

void kv::HumanInterface::ApplyActiveHandOnSlot(Slot* slot)
{
    Slot& active_hand = GetActiveHand();
    if (active_hand.item.IsValid() && !slot->item.IsValid())
    {
        if (!IsTypeMatch(*slot, active_hand.item->GetItemType()))
        {
            return;
        }
        slot->item = active_hand.item;
        active_hand.item = 0;
    }
    else if (!active_hand.item.IsValid() && slot->item.IsValid())
    {
        if (!IsTypeMatch(active_hand, slot->item->GetItemType()))
        {
            return;
        }
        active_hand.item = slot->item;
        slot->item = 0;
    }
    else if (slot->item.IsValid())
    {
        PerformAttack(slot->item, active_hand.item);
    }
}

void kv::HumanInterface::SwapHands()
{
    Slot& right_hand = GetSlot(slot::RIGHT_HAND);
    Slot& left_hand = GetSlot(slot::LEFT_HAND);
    Button& swap = GetButton(SWAP);

    active_hand_ = !active_hand_;
    if (!active_hand_)
    {
        right_hand.view.SetState(states::RIGHT_HAND_INACTIVE);
        left_hand.view.SetState(states::LEFT_HAND_ACTIVE);
        swap.view.SetState(states::LEFT_HAND);
    }
    else
    {
        right_hand.view.SetState(states::RIGHT_HAND_ACTIVE);
        left_hand.view.SetState(states::LEFT_HAND_INACTIVE);
        swap.view.SetState(states::RIGHT_HAND);
    }
}

kv::Button& kv::HumanInterface::GetButton(const QString& button_name)
{
    for (Button& button : buttons_)
    {
        if (button.name == button_name)
        {
            return button;
        }
    }
    kv::Abort(QString("No such button in HumanInterface: %1").arg(button_name));
}
