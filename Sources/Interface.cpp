#include "Interface.h"

#include "Item.h"

void HumanInterface::InitSlots()
{
    r_hand_.SetPos(0, 14);
    r_hand_.GetView()->SetState("hand_r_inactive");

    drop_.SetPos(7, 15);
    drop_.GetView()->SetState("act_drop");
}

id_ptr_on<Item> HumanInterface::Click(int x, int y)
{
    x = static_cast<int>(static_cast<float>(x) * (static_cast<float>(sizeW + guiShift) / static_cast<float>(GetScreen()->w())));;
    y = static_cast<int>(static_cast<float>(y) * (static_cast<float>(sizeH) / static_cast<float>(GetScreen()->h())));

    if (r_hand_.Click(x, y))
        return r_hand_.Get();
    if (drop_.Click(x, y))
    {
        Message msg;
        msg.text = "SDLK_p";
        NetClient::GetNetClient()->Send(msg);
        return 0;
    }
    return 0;
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
    return false;
}

HumanInterface::~HumanInterface()
{
}

void HumanInterface::Draw()
{
    r_hand_.Draw();
    drop_.Draw();
}

unsigned int HumanInterface::hash() const
{
    unsigned int hash = 0;
    hash += r_hand_.hash_member();
    hash += drop_.hash_member();
    return hash;
}

void HumanInterface::Pick(id_ptr_on<Item> item)
{
    if (r_hand_.Get())
        return;
    r_hand_.Set(item);
}

void HumanInterface::Drop()
{
    r_hand_.Remove();
}

id_ptr_on<Item> HumanInterface::GetRHand()
{
    return r_hand_.Get();
}

std::ostream& operator<<(std::stringstream& file, HumanInterface& interf)
{
    interf.r_hand_.operator<<(file) << " ";
    interf.drop_.operator<<(file) << " ";
    return file;
}
std::istream& operator>>(std::stringstream& file, HumanInterface& interf)
{
    interf.r_hand_.operator>>(file);
    interf.drop_.operator>>(file);
    return file;
}