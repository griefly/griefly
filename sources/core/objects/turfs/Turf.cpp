#include "Turf.h"

#include "core/objects/movable/structures/Lattice.h"
#include "core/objects/Tile.h"
#include "representation/Representation.h"

using namespace kv;

Turf::Turf()
{
    friction_ = friction::BASE_FRICTION;
    atmos_state_ = atmos::SIMULATED;
}

void Turf::Delete()
{
    MaterialObject::Delete();
}

void Turf::Represent()
{
    if (IdPtr<CubeTile> tile = GetOwner())
    {
        atmos::AtmosHolder* holder = tile->GetAtmosHolder();
        int plasma = holder->GetGase(atmos::PLASMA);
        const int VISIBILITY_THRESHOLD = 5;
        if (plasma > VISIBILITY_THRESHOLD)
        {
            Representation::Entity ent;
            ent.id = GetRepresentation().GetUniqueIdForNewFrame(GetId(), 1);
            ent.click_id = 0;
            ent.pos_x = GetPosition().x;
            ent.pos_y = GetPosition().y;
            ent.vlevel = 11;
            ent.view.SetSprite("icons/plasma.dmi");
            ent.view.SetState("plasma");
            const double FULL_VISIBILITY_THRESHOLD = 100.0;
            const double visibility = (plasma * 1.0) * (MAX_TRANSPARENCY / FULL_VISIBILITY_THRESHOLD);
            ent.view.SetTransparency(qMin(static_cast<int>(visibility), MAX_TRANSPARENCY));
            ent.dir = Dir::DOWN;
            GetRepresentation().AddToNewFrame(ent);
        }
    }
    MaterialObject::Represent();
}

int friction::CombinedFriction(IdPtr<Turf> turf)
{
    if (   turf->GetPosition().x == 0
        || turf->GetPosition().y == 0
        || turf->GetPosition().x == turf->GetGame().GetMap().GetWidth() - 1
        || turf->GetPosition().y == turf->GetGame().GetMap().GetHeight() - 1)
    {
        return friction::BASE_FRICTION;
    }

    int retval =
          turf->GetFriction()
        + turf->GetNeighbour(Dir::UP)->GetTurf()->GetFriction()
        + turf->GetNeighbour(Dir::DOWN)->GetTurf()->GetFriction()
        + turf->GetNeighbour(Dir::LEFT)->GetTurf()->GetFriction()
        + turf->GetNeighbour(Dir::RIGHT)->GetTurf()->GetFriction();
    if (retval > friction::BASE_FRICTION)
    {
        retval = friction::BASE_FRICTION;
    }

    // TODO: Remake

    if (retval < friction::BASE_FRICTION)
    {
        if (   turf->GetOwner()->GetItem<Lattice>()
            || turf->GetNeighbour(Dir::UP)->GetItem<Lattice>()
            || turf->GetNeighbour(Dir::DOWN)->GetItem<Lattice>()
            || turf->GetNeighbour(Dir::LEFT)->GetItem<Lattice>()
            || turf->GetNeighbour(Dir::RIGHT)->GetItem<Lattice>()
            )
        {
            retval = friction::BASE_FRICTION;
        }
    }

    return retval;
}
