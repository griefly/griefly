#include "HumanInterface.h"

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
    // TODO
}

IdPtr<Item> HumanInterface2::GetItem(const QString& slot)
{
    // TODO
}

bool HumanInterface2::RemoveItem(const QString& slot)
{
    // TODO
}

bool HumanInterface2::InsertItem(const QString& slot, IdPtr<Item> item)
{
    // TODO
}
