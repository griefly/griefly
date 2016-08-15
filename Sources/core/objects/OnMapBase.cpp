#include "OnMapBase.h"
#include "Turf.h"
#include "Movable.h"
#include "Item.h"
#include "../ObjectFactory.h"

IdPtr<ITurf> IOnMapBase::GetTurf()
{
    if (owner.IsValid())
    {
        return owner->GetTurf();
    }
    return 0;
}

IdPtr<CubeTile> IOnMapBase::GetTile()
{
    IdPtr<IOnMapBase> item = GetId();
    if (item.IsValid())
    {
        if (!item->GetOwner().IsValid())
        {
            return item;
        }
        else
        {
            return item->GetOwner()->GetTile();
        }
    }
}


void IOnMapBase::SetTurf(IdPtr<ITurf> turf)
{
    if (owner.IsValid())
    {
        owner->SetTurf(turf);
    }
}

void IOnMapBase::Bump(IdPtr<IMovable> item)
{
   // nothing
}

void IOnMapBase::BumpByGas(Dir dir, bool inside)
{
   // nothing
}

void IOnMapBase::AttackBy(IdPtr<Item> item)
{
    // nothing
}
