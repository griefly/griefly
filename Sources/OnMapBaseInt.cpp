#include "OnMapBaseInt.h"
#include "ITurf.h"

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