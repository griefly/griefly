#include "Interface.h"

#include "Item.h"
#include "helpers.h"

void HumanInterface::InitSlots()
{
    r_hand_.SetPos(0, 14);
    r_hand_.GetView()->SetState("hand_r_active");

    l_hand_.SetPos(2, 14);
    l_hand_.GetView()->SetState("hand_l_inactive");

    head_.SetPos(1, 13);
    head_.GetView()->SetState("head");

    suit_.SetPos(1, 14);
    suit_.GetView()->SetState("suit");
    suit_.SetType("SUIT");

    active_hand_ = true;

    drop_.SetPos(7, 15);
    drop_.GetView()->SetState("act_drop");

    swap_.SetPos(6, 15);
    swap_.GetView()->SetSprite("icons/screen1_old.dmi");
    swap_.GetView()->SetState("hand");
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

const std::string RIGHT_HAND = "INT_RHAND";
const std::string LEFT_HAND = "INT_LHAND";
const std::string HEAD = "HEAD";
const std::string SUIT = "SUIT";
const std::string DROP = "DROP";
const std::string SWAP = "SWAP";

bool HumanInterface::Click(int x, int y)
{
    helpers::normalize_pixels(&x, &y);
    Message msg;
    if (r_hand_.Click(x, y))
    {
        msg.text = RIGHT_HAND;
    }
    else if (l_hand_.Click(x, y))
    {
        msg.text = LEFT_HAND;
    }
    else if (head_.Click(x, y))
    {
        msg.text = HEAD;
    }
    else if (suit_.Click(x, y))
    {
        msg.text = SUIT;
    }
    else if (drop_.Click(x, y))
    {
        msg.text = DROP;
    }
    else if (swap_.Click(x, y))
    {
        msg.text = SWAP;
    }
    else
    {
        return false;
    }
    NetClient::GetNetClient()->Send(msg);
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

void HumanInterface::HandleClick(const std::string& place)
{
    if (place == RIGHT_HAND)
    {
        ApplyActiveHandOnSlot(&r_hand_);
    }
    else if (place == LEFT_HAND)
    {
        ApplyActiveHandOnSlot(&l_hand_);
    }
    else if (place == HEAD)
    {
        ApplyActiveHandOnSlot(&head_);
    }
    else if (place == SUIT)
    {
        ApplyActiveHandOnSlot(&suit_);
    }
    else if (place == DROP)
    {
        if(GetActiveHand().Get())
        {
            owner_->GetOwner()->AddItem(GetActiveHand().Get());
            Drop();
        }
    }
    else if (place == SWAP)
    {
        SwapHands();
    }
    SYSTEM_STREAM << "Inteface click: " << place << std::endl;
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
    swap_.Draw(helpers::dir_to_byond(active_hand_ ? D_UP : D_DOWN));
}

unsigned int HumanInterface::hash() const
{
    unsigned int hash = 0;
    hash += r_hand_.hash_member();
    hash += drop_.hash_member();
    hash += l_hand_.hash_member();
    hash += head_.hash_member();
    hash += suit_.hash_member();
    hash += active_hand_;
    hash += swap_.hash_member();
    hash += owner_.ret_id();
    return hash;
}

void HumanInterface::Pick(id_ptr_on<Item> item)
{
    if (GetActiveHand().Get())
        return;
    GetActiveHand().Set(item);
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
    interf.swap_.operator<<(file) << " ";
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
    interf.swap_.operator>>(file);
    file >> interf.active_hand_;
    file >> interf.owner_;
    return file;
}
