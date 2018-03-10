#include "MapObject.h"
#include "turfs/Turf.h"
#include "movable/Movable.h"
#include "movable/items/Item.h"
#include "../ObjectFactory.h"

using namespace kv;

IdPtr<Turf> MapObject::GetTurf()
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

void MapObject::BumpByGas(Dir dir, bool inside)
{
   // nothing
}

void MapObject::AttackBy(IdPtr<Item> item)
{
    // nothing
}
