#include "Closet.h"

#include "Human.h"
#include "sound.h"

Closet::Closet(size_t id)
    : IMovable(id)
{
    v_level = 4;

    name = "Closet";

    open_ = false;
    SetPassable(D_ALL, Passable::AIR);
    SetPassable(D_UP, Passable::AIR);
    SetPassable(D_DOWN, Passable::AIR);
    SetPassable(D_LEFT, Passable::AIR);
    SetPassable(D_RIGHT, Passable::AIR);

    SetSprite("icons/closet.dmi");
    SetState("closed");
}

bool Closet::Contains(id_ptr_on<IOnMapBase> item) const
{
    for (auto it = content_.begin(); it != content_.end(); ++it)
    {
        if (it->ret_id() == item.ret_id())
            return true;
    }
    return false;
}

void Closet::AttackBy(id_ptr_on<Item> item)
{
    if (item)
        return;

    if (open_)
        Close();
    else
        Open();
}

void Closet::Bump(id_ptr_on<IMovable> item)
{
    if (Contains(item))
    {
        if (!open_)
        {
            Open();
        }
        return;
    }
    IMovable::Bump(item);
}

bool Closet::AddItem(id_ptr_on<IOnMapBase> item)
{
    if (id_ptr_on<Item> i = item)
    {
        content_.push_back(item);
        item->SetOwner(GetId());
        return true;
    }
    if (id_ptr_on<Human> h = item)
    {
        content_.push_back(item);
        item->SetOwner(GetId());
        return true;
    }
    return false;
}

bool Closet::RemoveItem(id_ptr_on<IOnMapBase> item)
{
    auto it = content_.begin();
    for (; it != content_.end(); ++it)
    {
        if (*it == item)
        {
            content_.erase(it);
            return true;
        }
    }
    return false;
}

void Closet::Close()
{
    owner->ForEach([this](id_ptr_on<IOnMapBase> item)
    {
        if (AddItem(item))
        {
            owner->RemoveItem(item);
        }
    });

    open_ = false;
    SetPassable(D_ALL, Passable::AIR);
    SetPassable(D_UP, Passable::AIR);
    SetPassable(D_DOWN, Passable::AIR);
    SetPassable(D_LEFT, Passable::AIR);
    SetPassable(D_RIGHT, Passable::AIR);
    SetState("closed");

    PlaySoundIfVisible("click.ogg", owner.ret_id());
}

void Closet::Open()
{
    for (auto it = content_.begin(); it != content_.end(); ++it)
    {
        owner->AddItem(*it);
    }
    content_.clear();

    open_ = true;
    SetPassable(D_ALL, Passable::FULL);
    SetPassable(D_UP, Passable::FULL);
    SetPassable(D_DOWN, Passable::FULL);
    SetPassable(D_LEFT, Passable::FULL);
    SetPassable(D_RIGHT, Passable::FULL);
    SetState("open");

    PlaySoundIfVisible("click.ogg", owner.ret_id());
}
