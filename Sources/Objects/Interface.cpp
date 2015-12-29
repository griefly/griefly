#include "Interface.h"

#include "Item.h"
#include "helpers.h"

#include "Human.h"

void HumanInterface::InitSlots()
{
    r_hand_.SetPos(0, 14);
    r_hand_.GetView()->SetState("hand_r_active");
    r_hand_.SetName(HumanInterfacePlaces::RIGHT_HAND);

    l_hand_.SetPos(2, 14);
    l_hand_.GetView()->SetState("hand_l_inactive");
    l_hand_.SetName(HumanInterfacePlaces::LEFT_HAND);

    head_.SetPos(1, 13);
    head_.GetView()->SetState("head");
    head_.SetType(Slots::HEAD);
    head_.SetName(HumanInterfacePlaces::HEAD);

    suit_.SetPos(1, 14);
    suit_.GetView()->SetState("suit");
    suit_.SetType(Slots::SUIT);
    suit_.SetName(HumanInterfacePlaces::SUIT);

    uniform_.SetPos(1, 15);
    uniform_.GetView()->SetState("uniform");
    uniform_.SetType(Slots::UNIFORM);
    uniform_.SetName(HumanInterfacePlaces::UNIFORM);

    feet_.SetPos(3, 14);
    feet_.GetView()->SetState("shoes");
    feet_.SetType(Slots::FEET);
    feet_.SetName(HumanInterfacePlaces::FEET);

    active_hand_ = true;

    drop_.SetPos(7, 15);
    drop_.GetView()->SetState("act_drop");
    drop_.SetName(HumanInterfacePlaces::DROP);

    swap_.SetPos(6, 15);
    swap_.GetView()->SetSprite("icons/screen1_old.dmi");
    swap_.GetView()->SetState("hand");
    swap_.SetName(HumanInterfacePlaces::SWAP);

    health_.SetPos(15, 10);
    health_.GetView()->SetSprite("icons/screen1_old.dmi");
    health_.GetView()->SetState("health0");
    health_.SetName(HumanInterfacePlaces::UNCLICKABLE);

    lay_.SetPos(15, 11);
    lay_.GetView()->SetSprite("icons/screen1_old.dmi");
    lay_.GetView()->SetState("rest0");
    lay_.SetName(HumanInterfacePlaces::LAY);

    if (id_ptr_on<Human> owner = owner_)
    {
        owner->UpdateOverlays();
    }
}

void HumanInterface::SwapHands()
{
    if (active_hand_)
    {
        r_hand_.GetView()->SetState("hand_r_inactive");
        l_hand_.GetView()->SetState("hand_l_active");
        active_hand_ = false;
    }
    else
    {
        r_hand_.GetView()->SetState("hand_r_active");
        l_hand_.GetView()->SetState("hand_l_inactive");
        active_hand_ = true;
    }
}

Slot<Item>& HumanInterface::GetActiveHand()
{
    if (active_hand_)
        return r_hand_;
    else
        return l_hand_;
}

void HumanInterface::UpdateLaying()
{
    if (id_ptr_on<Human> human = owner_)
    {
        if (human->GetLying() == true)
        {
            lay_.GetView()->SetState("rest1");
        }
        else
        {
            lay_.GetView()->SetState("rest0");
        }
    }
}

void HumanInterface::UpdateHealth()
{
    if (id_ptr_on<Human> human = owner_)
    {
        int health = human->GetHealth();
        if (health == 100)
        {
            health_.GetView()->SetState("health0");
        }
        else if (health >= 80)
        {
            health_.GetView()->SetState("health1");
        }
        else if (health >= 60)
        {
            health_.GetView()->SetState("health2");
        }
        else if (health >= 40)
        {
            health_.GetView()->SetState("health3");
        }
        else if (health >= 20)
        {
            health_.GetView()->SetState("health4");
        }
        else if (health >= 0)
        {
            health_.GetView()->SetState("health5");
        }
        else //if (health_.GetView()->GetBaseFrameset()->GetState() != "health6")
        {
            health_.GetView()->SetState("health6");
        }
    }
}

bool HumanInterface::Click(int x, int y)
{
    helpers::normalize_pixels(&x, &y);
    std::string msg;
    if (r_hand_.Click(x, y))
    {
        msg = HumanInterfacePlaces::RIGHT_HAND;
    }
    else if (l_hand_.Click(x, y))
    {
        msg = HumanInterfacePlaces::LEFT_HAND;
    }
    else if (head_.Click(x, y))
    {
        msg = HumanInterfacePlaces::HEAD;
    }
    else if (suit_.Click(x, y))
    {
        msg = HumanInterfacePlaces::SUIT;
    }
    else if (feet_.Click(x, y))
    {
        msg = HumanInterfacePlaces::FEET;
    }
    else if (uniform_.Click(x, y))
    {
        msg = HumanInterfacePlaces::UNIFORM;
    }
    else if (drop_.Click(x, y))
    {
        msg = HumanInterfacePlaces::DROP;
    }
    else if (swap_.Click(x, y))
    {
        msg = HumanInterfacePlaces::SWAP;
    }
    else if (lay_.Click(x, y))
    {
        msg = HumanInterfacePlaces::LAY;
    }
    else if (health_.Click(x, y))
    {
        return true;
    }
    else
    {
        return false;
    }

    Network2::GetInstance().SendOrdinaryMessage(QString::fromStdString(msg));

    return true;
}

void HumanInterface::ApplyActiveHandOnSlot(Slot<Item>* slot)
{
    if (GetActiveHand().Get() && !slot->Get())
    {
        if (!slot->MatchType(GetActiveHand().Get()->type))
            return;
        slot->Set(GetActiveHand().Get());
        GetActiveHand().Remove();
    }
    else if (slot->Get() && !GetActiveHand().Get())
    {
        if (!GetActiveHand().MatchType(slot->Get()->type))
            return;
        GetActiveHand().Set(slot->Get());
        slot->Remove();
    }
    else if (slot->Get())
    {
        slot->Get()->AttackBy(GetActiveHand().Get());
    }
}

void HumanInterface::AddOverlays()
{
    if (uniform_.Get())
    {
        std::string state_name = uniform_.Get()->GetView()->GetBaseFrameset()->GetState();
        owner_->GetView()->AddOverlay("icons/uniform.dmi", state_name + "_s");
    }
    if (feet_.Get())
    {
        std::string state_name = feet_.Get()->GetView()->GetBaseFrameset()->GetState();
        owner_->GetView()->AddOverlay("icons/feet.dmi", state_name);
    }
    if (head_.Get())
    {
        std::string state_name = head_.Get()->GetView()->GetBaseFrameset()->GetState();
        owner_->GetView()->AddOverlay("icons/head.dmi", state_name);
    }
    if (suit_.Get())
    {
        std::string state_name = suit_.Get()->GetView()->GetBaseFrameset()->GetState();
        owner_->GetView()->AddOverlay("icons/suit.dmi", state_name);
    }

    if (r_hand_.Get())
    {
        std::string state_name = r_hand_.Get()->GetView()->GetBaseFrameset()->GetState();
        owner_->GetView()->AddOverlay("icons/items_righthand.dmi", state_name);
    }

    if (l_hand_.Get())
    {
        std::string state_name = l_hand_.Get()->GetView()->GetBaseFrameset()->GetState();
        owner_->GetView()->AddOverlay("icons/items_lefthand.dmi", state_name);
    }
}

void HumanInterface::HandleClick(const std::string& place)
{
    if (place == HumanInterfacePlaces::RIGHT_HAND)
    {
        ApplyActiveHandOnSlot(&r_hand_);
    }
    else if (place == HumanInterfacePlaces::LEFT_HAND)
    {
        ApplyActiveHandOnSlot(&l_hand_);
    }
    else if (place == HumanInterfacePlaces::HEAD)
    {
        ApplyActiveHandOnSlot(&head_);
    }
    else if (place == HumanInterfacePlaces::SUIT)
    {
        ApplyActiveHandOnSlot(&suit_);
    }
    else if (place == HumanInterfacePlaces::FEET)
    {
        ApplyActiveHandOnSlot(&feet_);
    }
    else if (place == HumanInterfacePlaces::UNIFORM)
    {
        ApplyActiveHandOnSlot(&uniform_);
    }
    else if (place == HumanInterfacePlaces::DROP)
    {
        qDebug() << "Drop";
        qDebug() << owner_.ret_id();
        qDebug() << owner_->GetOwner().ret_id();
        if (GetActiveHand().Get())
        {
            qDebug() << "Begin add item";
            owner_->GetOwner()->AddItem(GetActiveHand().Get());
            qDebug() << "End add item";
            Drop();
            qDebug() << "End drop item";
        }
    }
    else if (place == HumanInterfacePlaces::LAY)
    {
        if (id_ptr_on<Human> owner = owner_)
        {
            bool laying_ = owner->GetLying();
            if (laying_)
            {
                owner->SetLying(false);
            }
            else
            {
                owner->AddLyingTimer(50);
                owner->SetLying(true);
            }
        }
    }
    else if (place == HumanInterfacePlaces::SWAP)
    {
        SwapHands();
    }

    if (id_ptr_on<Human> owner = owner_)
    {
        owner->UpdateOverlays();
    }

    //SYSTEM_STREAM << "Inteface click: " << place << std::endl;
}

HumanInterface::~HumanInterface()
{
}

void HumanInterface::Draw()
{
    r_hand_.Draw();
    l_hand_.Draw();
    drop_.Draw();
    head_.Draw();
    suit_.Draw();
    uniform_.Draw();
    feet_.Draw();
    swap_.Draw(helpers::dir_to_byond(active_hand_ ? D_UP : D_DOWN));
    health_.Draw();
    lay_.Draw();
}

unsigned int HumanInterface::hash() const
{
    unsigned int hash = 0;
    hash += r_hand_.hash_member();
    hash += drop_.hash_member();
    hash += l_hand_.hash_member();
    hash += head_.hash_member();
    hash += suit_.hash_member();
    hash += feet_.hash_member();
    hash += uniform_.hash_member();
    hash += active_hand_;
    hash += swap_.hash_member();
    hash += health_.hash_member();
    hash += lay_.hash_member();
    hash += owner_.ret_id();
    return hash;
}

void HumanInterface::Pick(id_ptr_on<Item> item)
{
    if (GetActiveHand().Get())
        return;
    GetActiveHand().Set(item);
    if (id_ptr_on<Human> owner = owner_)
    {
        owner->UpdateOverlays();
    }
}

void HumanInterface::Drop()
{
    GetActiveHand().Remove();
}

id_ptr_on<Item> HumanInterface::GetRHand()
{
    return r_hand_.Get();
}

id_ptr_on<Item> HumanInterface::GetLHand()
{
    return l_hand_.Get();
}

std::ostream& operator<<(std::stringstream& file, HumanInterface& interf)
{
    interf.r_hand_.operator<<(file) << " ";
    interf.drop_.operator<<(file) << " ";
    interf.l_hand_.operator<<(file) << " ";
    interf.head_.operator<<(file) << " ";
    interf.suit_.operator<<(file) << " ";
    interf.uniform_.operator<<(file) << " ";
    interf.feet_.operator<<(file) << " ";
    interf.swap_.operator<<(file) << " ";
    interf.health_.operator<<(file) << " ";
    interf.lay_.operator<<(file) << " ";
    file << interf.active_hand_ << " ";
    file << interf.owner_ << " ";
    return file;
}
std::istream& operator>>(std::stringstream& file, HumanInterface& interf)
{
    interf.r_hand_.operator>>(file);
    interf.drop_.operator>>(file);
    interf.l_hand_.operator>>(file);
    interf.head_.operator>>(file);
    interf.suit_.operator>>(file);
    interf.uniform_.operator>>(file);
    interf.feet_.operator>>(file);
    interf.swap_.operator>>(file);
    interf.health_.operator>>(file);
    interf.lay_.operator>>(file);
    file >> interf.active_hand_;
    file >> interf.owner_;
    return file;
}
