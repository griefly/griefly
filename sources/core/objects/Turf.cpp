#include "Turf.h"

#include "Lattice.h"
#include "Tile.h"
#include "representation/Representation.h"

ITurf::ITurf(quint32 id) : IOnMapObject(id)
{
    friction_ = Friction::BASE_FRICTION;
    atmos_state_ = atmos::SIMULATED;
}

void ITurf::Delete() 
{
    IOnMapObject::Delete();
}

void ITurf::Represent()
{
    if (IdPtr<CubeTile> tile = owner)
    {
        atmos::AtmosHolder* holder = tile->GetAtmosHolder();
        int plasma = holder->GetGase(atmos::PLASMA);
        const int VISIBLE_PLASMA_TRESHOLD = 75;
        if (plasma > VISIBLE_PLASMA_TRESHOLD)
        {
            Representation::Entity ent;
            // TODO: dirty hack, fix me
            ent.id = GetId() + 1000000;
            ent.pos_x = GetX();
            ent.pos_y = GetY();
            ent.vlevel = 11;
            ent.view.SetSprite("icons/plasma.dmi");
            ent.view.SetState("plasma");
            ent.clickable = false;
            ent.dir = D_DOWN;
            GetRepresentation().AddToNewFrame(ent);
        }
    }
    IOnMapObject::Represent();
}

int Friction::CombinedFriction(IdPtr<ITurf> turf)
{
    if (   turf->GetX() == 0
        || turf->GetY() == 0
        || turf->GetX() == turf->GetGame().GetMap().GetWidth() - 1
        || turf->GetY() == turf->GetGame().GetMap().GetHeight() - 1)
    {
        return Friction::BASE_FRICTION;
    }

    int retval =
          turf->GetFriction()
        + turf->GetNeighbour(D_UP)->GetTurf()->GetFriction()
        + turf->GetNeighbour(D_DOWN)->GetTurf()->GetFriction()
        + turf->GetNeighbour(D_LEFT)->GetTurf()->GetFriction()
        + turf->GetNeighbour(D_RIGHT)->GetTurf()->GetFriction();
    if (retval > Friction::BASE_FRICTION)
    {
        retval = Friction::BASE_FRICTION;
    }

    // TODO: Remake

    if (retval < Friction::BASE_FRICTION)
    {
        if (   turf->GetOwner()->GetItem<Lattice>()
            || turf->GetNeighbour(D_UP)->GetItem<Lattice>()
            || turf->GetNeighbour(D_DOWN)->GetItem<Lattice>()
            || turf->GetNeighbour(D_LEFT)->GetItem<Lattice>()
            || turf->GetNeighbour(D_RIGHT)->GetItem<Lattice>()
            )
        {
            retval = Friction::BASE_FRICTION;
        }
    }

    return retval;
}
