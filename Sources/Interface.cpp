#include "Interface.h"

#include "Item.h"
#include "helpers.h"

void HumanInterface::InitSlots()
{
    r_hand_.SetPos(0, 14);
    r_hand_.GetView()->SetState("hand_r_active");

    l_hand_.SetPos(2, 14);
    l_hand_.GetView()->SetState("hand_l_inactive");

    active_hand_ = true;

    drop_.SetPos(7, 15);
    drop_.GetView()->SetState("act_drop");
}

id_ptr_on<Item> HumanInterface::Click(int x, int y)
{
    helpers::normalize_pixels(&x, &y);
    if (r_hand_.Click(x, y))
        return r_hand_.Get();
    if (l_hand_.Click(x, y))
        return l_hand_.Get();
    if (drop_.Click(x, y))
    {
        Message msg;
        msg.text = "SDLK_p";
        NetClient::GetNetClient()->Send(msg);
        return 0;
    }
    return 0;
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

bool HumanInterface::IsArea(int x, int y)
{
    helpers::normalize_pixels(&x, &y);    
    return    r_hand_.Click(x, y)
           || drop_.Click(x, y);
}

bool HumanInterface::HandleClick(id_ptr_on<Item> item)
{
    if (!item)
        return false;

    if (item == r_hand_.Get())
    {
        item->AttackBy(0);
        return true;
    }
    if (item == l_hand_.Get())
    {
        item->AttackBy(0);
        return true;
    }
    return false;
}

HumanInterface::~HumanInterface()
{
}

void HumanInterface::Draw()
{
    r_hand_.Draw();
    l_hand_.Draw();
    drop_.Draw();
}

unsigned int HumanInterface::hash() const
{
    unsigned int hash = 0;
    hash += r_hand_.hash_member();
    hash += drop_.hash_member();
    hash += l_hand_.hash_member();
    hash += active_hand_;
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
    file << interf.active_hand_ << " ";
    return file;
}
std::istream& operator>>(std::stringstream& file, HumanInterface& interf)
{
    interf.r_hand_.operator>>(file);
    interf.drop_.operator>>(file);
    interf.l_hand_.operator>>(file);
    file >> interf.active_hand_;
    return file;
}