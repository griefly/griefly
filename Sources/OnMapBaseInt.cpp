#include "OnMapBaseInt.h"
#include "ITurf.h"
#include "IMovable.h"
#include "Item.h"

id_ptr_on<ITurf> IOnMapBase::GetTurf()
{
    if (owner.valid())
        return owner->GetTurf();
    return 0;
}

void IOnMapBase::SetTurf(id_ptr_on<ITurf> turf)
{
    if (owner.valid())
        owner->SetTurf(turf);
}

void IOnMapBase::Bump(id_ptr_on<IMovable> item)
{
   // nothing
}

void IOnMapBase::AttackBy(id_ptr_on<Item> item)
{
    // nothing
}