#include "ITurf.h"

#include "Lattice.h"

ITurf::ITurf(size_t id) : IOnMapObject(id)
{
    friction_ = Friction::BASE_FRICTION;
    atmos_state_ = SIMULATED;
}

void ITurf::delThis() 
{
    IOnMapObject::delThis();
}

int Friction::CombinedFriction(id_ptr_on<ITurf> turf)
{
    int retval =
          turf->GetFriction()
        + turf->GetNeighbour(D_UP)->GetTurf()->GetFriction()
        + turf->GetNeighbour(D_DOWN)->GetTurf()->GetFriction()
        + turf->GetNeighbour(D_LEFT)->GetTurf()->GetFriction()
        + turf->GetNeighbour(D_RIGHT)->GetTurf()->GetFriction();
    if (retval > Friction::BASE_FRICTION)
        retval = Friction::BASE_FRICTION;

    // TODO: Remake

    if (   turf->GetItem<Lattice>()
        || turf->GetNeighbour(D_UP)->GetItem<Lattice>()
        || turf->GetNeighbour(D_DOWN)->GetItem<Lattice>()
        || turf->GetNeighbour(D_LEFT)->GetItem<Lattice>()
        || turf->GetNeighbour(D_RIGHT)->GetItem<Lattice>()
        )
        retval = Friction::BASE_FRICTION;

    return retval;
}
