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

void Turf::Represent(Representation* representation)
{
    if (IdPtr<CubeTile> tile = GetOwner())
    {
        atmos::AtmosHolder* holder = tile->GetAtmosHolder();
        int plasma = holder->GetGase(atmos::PLASMA);
        const int VISIBILITY_THRESHOLD = 5;
        if (plasma > VISIBILITY_THRESHOLD)
        {
            FrameData::Entity entity;
            entity.id = representation->GetUniqueIdForNewFrame(GetId(), 1);
            entity.click_id = 0;
            entity.pos_x = GetPosition().x;
            entity.pos_y = GetPosition().y;
            entity.vlevel = 11;
            entity.view.SetSprite("icons/plasma.dmi");
            entity.view.SetState("plasma");
            const double FULL_VISIBILITY_THRESHOLD = 100.0;
            const double visibility = (plasma * 1.0) * (MAX_TRANSPARENCY / FULL_VISIBILITY_THRESHOLD);
            entity.view.SetTransparency(qMin(static_cast<int>(visibility), MAX_TRANSPARENCY));
            entity.dir = Dir::SOUTH;
            representation->AddToNewFrame(entity);
        }
    }
    MaterialObject::Represent(representation);
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
        + turf->GetNeighbour(Dir::NORTH)->GetTurf()->GetFriction()
        + turf->GetNeighbour(Dir::SOUTH)->GetTurf()->GetFriction()
        + turf->GetNeighbour(Dir::WEST)->GetTurf()->GetFriction()
        + turf->GetNeighbour(Dir::EAST)->GetTurf()->GetFriction();
    if (retval > friction::BASE_FRICTION)
    {
        retval = friction::BASE_FRICTION;
    }

    // TODO: Remake

    if (retval < friction::BASE_FRICTION)
    {
        if (   turf->GetOwner()->GetItem<Lattice>()
            || turf->GetNeighbour(Dir::NORTH)->GetItem<Lattice>()
            || turf->GetNeighbour(Dir::SOUTH)->GetItem<Lattice>()
            || turf->GetNeighbour(Dir::WEST)->GetItem<Lattice>()
            || turf->GetNeighbour(Dir::EAST)->GetItem<Lattice>()
            )
        {
            retval = friction::BASE_FRICTION;
        }
    }

    return retval;
}
