#include "Interface.h"

#include "Item.h"

void HumanInterface::InitSlots()
{
    auto r_hand = new Slot<Item>;
    r_hand->SetPos(3, 3);
    r_hand->GetView()->SetState("hand_r_inactive");
    slots_.push_back(r_hand);
}

bool HumanInterface::Click(int x, int y)
{
    return false;
}

HumanInterface::~HumanInterface()
{
    for (auto it = slots_.begin(); it != slots_.end(); ++it)
        delete *it;
}

void HumanInterface::Draw()
{
    for (auto it = slots_.begin(); it != slots_.end(); ++it)
        (*it)->Draw();
}

std::ostream& HumanInterface::operator<<(std::stringstream& file)
{
    file << slots_.size() << " ";
    for (size_t i = 0; i < slots_.size(); ++i)
        file << slots_[i] << " ";
    return file;
}
std::istream& HumanInterface::operator>>(std::stringstream& file)
{
    size_t s;
    file >> s;
    slots_.resize(s);
    for (size_t i = 0; i <slots_.size(); ++i)
    {

    }
}