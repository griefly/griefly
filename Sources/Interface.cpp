#include "Interface.h"

#include "Item.h"

void HumanInterface::InitSlots()
{
    r_hand_.SetPos(3, 3);
    r_hand_.GetView()->SetState("hand_r_inactive");
}

bool HumanInterface::Click(int x, int y)
{
    return false;
}

HumanInterface::~HumanInterface()
{
}

void HumanInterface::Draw()
{
    r_hand_.Draw();
}

unsigned int HumanInterface::hash() const
{
    unsigned int hash = 0;
    hash += r_hand_.hash_member();
    return hash;
}

std::ostream& HumanInterface::operator<<(std::stringstream& file)
{
    r_hand_.operator<<(file) << " ";
    return file;
}
std::istream& HumanInterface::operator>>(std::stringstream& file)
{
    r_hand_.operator>>(file);
    return file;
}