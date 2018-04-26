#include "MapObject.h"
#include "turfs/Turf.h"
#include "movable/Movable.h"
#include "movable/items/Item.h"
#include "../ObjectFactory.h"

using namespace kv;

void kv::PerformAttack(IdPtr<MapObject> target, IdPtr<Item> item)
{
    if (item.IsValid())
    {
        item->Attack(target);
    }
    // 'item' or 'target' can be invalidated in the Attack method
    if (item.IsValid() && target.IsValid())
    {
        target->AttackBy(item);
    }
}

IdPtr<Turf> MapObject::GetTurf() const
{
    auto owner = GetOwner();
    if (owner.IsValid())
    {
        return owner->GetTurf();
    }
    return 0;
}

IdPtr<MapObject> MapObject::GetRoot()
{
    if (!GetOwner().IsValid())
    {
        return GetId();
    }
    return GetOwner()->GetRoot();
}

void MapObject::PlaySoundIfVisible(const QString& name)
{
    GetGame().AddSound(name, GetPosition());
}

void MapObject::SetTurf(IdPtr<Turf> turf)
{
    auto owner = GetOwner();
    if (owner.IsValid())
    {
        owner->SetTurf(turf);
    }
}

void MapObject::Bump(const Vector& force, IdPtr<Movable> item)
{
   // nothing
}

void MapObject::BumpByGas(const Vector& force, bool inside)
{
   // nothing
}

void MapObject::AttackBy(IdPtr<Item> item)
{
    // nothing
}
