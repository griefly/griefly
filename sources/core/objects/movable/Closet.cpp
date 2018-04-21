#include "Closet.h"

#include "objects/mobs/Human.h"

using namespace kv;

Closet::Closet()
{
    SetVisibleLevel(visible_level::ITEM - 1);

    SetName("Closet");

    open_ = false;

    SetPassable(Dir::ALL, passable::AIR);
    SetPassable(Dir::NORTH, passable::FULL);
    SetPassable(Dir::SOUTH, passable::FULL);
    SetPassable(Dir::WEST, passable::FULL);
    SetPassable(Dir::EAST, passable::FULL);

    SetSprite("icons/closet.dmi");
    SetState("closed");
}

bool Closet::Contains(IdPtr<MapObject> item) const
{
    for (auto it = content_.begin(); it != content_.end(); ++it)
    {
        if (it->Id() == item.Id())
        {
            return true;
        }
    }
    return false;
}

bool Closet::CanTouch(IdPtr<MapObject> item) const
{
    if (Contains(item))
    {
        return true;
    }
    return false;
}

void Closet::AttackBy(IdPtr<Item> item)
{
    if (item)
    {
        return;
    }

    if (open_)
    {
        Close();
    }
    else
    {
        Open();
    }
}

void Closet::Bump(const Vector& force, IdPtr<Movable> item)
{
    if (Contains(item))
    {
        if (!open_)
        {
            Open();
        }
        return;
    }
    Movable::Bump(force, item);
}

IdPtr<MapObject> Closet::GetNeighbour(Dir direct) const
{
    return GetId();
}

bool Closet::AddObject(IdPtr<MapObject> item)
{
    if (IdPtr<Item> i = item)
    {
        content_.push_back(item);
        item->SetOwner(GetId());
        return true;
    }
    if (IdPtr<Human> h = item)
    {
        content_.push_back(item);
        item->SetOwner(GetId());
        return true;
    }
    return false;
}

bool Closet::RemoveObject(IdPtr<MapObject> item)
{
    for (auto it = content_.begin(); it != content_.end(); ++it)
    {
        if (*it == item)
        {
            content_.erase(it);
            return true;
        }
    }
    return false;
}

void Closet::AfterWorldCreation()
{
    Movable::AfterWorldCreation();

    if (!GetOwner())
    {
        return;
    }

    GetOwner()->ForEach([this](IdPtr<MapObject> object)
    {
       if (AddObject(object))
       {
           GetOwner()->RemoveObject(object);
       }
    });
}

void Closet::Delete()
{
    QVector<IdPtr<Movable>> copy = content_;
    for (auto it = copy.begin(); it != copy.end(); ++it)
    {
        if (!it->IsValid())
        {
            kv::Abort("Closet contains invalid id_ptr_on");
        }
        (*it)->Delete();
    }
    Movable::Delete();
}

void Closet::Close()
{
    GetOwner()->ForEach([this](IdPtr<MapObject> item)
    {
        if (AddObject(item))
        {
            GetOwner()->RemoveObject(item);
        }
    });

    open_ = false;
    SetPassable(Dir::ALL, passable::AIR);
    SetState("closed");

    PlaySoundIfVisible("click.wav");
}

void Closet::Open()
{
    for (auto it = content_.begin(); it != content_.end(); ++it)
    {
        GetOwner()->AddObject(*it);
    }
    content_.clear();

    open_ = true;
    SetPassable(Dir::ALL, passable::FULL);
    SetState("open");

    PlaySoundIfVisible("click.wav");
}


SecurityLocker::SecurityLocker()
{
    locked_ = true;
    SetState("secure1");

    SetName("Security locker");
}

void SecurityLocker::AttackBy(IdPtr<Item> item)
{
    if (item.IsValid())
    {
        if (!open_)
        {
            if (locked_)
            {
                Unlock();
            }
            else
            {
                Lock();
            }
            return;
        }
    }
    Closet::AttackBy(item);
}

void SecurityLocker::Lock()
{
    if (open_)
    {
        return;
    }
    locked_ = true;
    SetState("secure1");
}

void SecurityLocker::Unlock()
{
    if (open_)
    {
        return;
    }
    locked_ = false;
    SetState("secure");
}

void SecurityLocker::Open()
{
    if (locked_)
    {
        return;
    }
    Closet::Open();
    SetState("secureopen");
}

void SecurityLocker::Close()
{
    Closet::Close();
    SetState("secure");
}
