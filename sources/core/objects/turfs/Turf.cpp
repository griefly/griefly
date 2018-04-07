#include "Turf.h"

#include "objects/movable/structures/Lattice.h"
#include "objects/Tile.h"

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

void Turf::Represent(GrowingFrame* frame, IdPtr<Mob> mob) const
{
    if (IdPtr<CubeTile> tile = GetOwner())
    {
        atmos::AtmosHolder* holder = tile->GetAtmosHolder();
        const int plasma = holder->GetGase(atmos::PLASMA);
        const int oxygen = holder->GetGase(atmos::OXYGEN);
        const int VISIBILITY_THRESHOLD = 5;
        if (holder->IsBurning() && plasma && oxygen)
        {
            const int intensity = std::min(plasma, oxygen / atmos::O2_MULTIPLIER);

            FrameData::Entity entity;
            entity.id = frame->GetUniqueIdForNewFrame(GetId(), 2);

            entity.click_id = 0;
            entity.pos_x = GetPosition().x;
            entity.pos_y = GetPosition().y;
            entity.vlevel = 3;
            entity.view.base_frameset.sprite_name = "icons/fire.dmi";
            if (intensity > 100)
            {
                entity.view.base_frameset.state = "3";
            }
            else if (intensity > 50)
            {
                entity.view.base_frameset.state = "2";
            }
            else
            {
                entity.view.base_frameset.state = "1";
            }
            entity.view.transparency = MAX_TRANSPARENCY;
            switch (GetId() % 4)
            {
            case 0:
                entity.dir = Dir::SOUTH;
                break;
            case 1:
                entity.dir = Dir::NORTH;
                break;
            case 2:
                entity.dir = Dir::WEST;
                break;
            case 3:
                entity.dir = Dir::EAST;
                break;
            }
            frame->Append(entity);
        }
        else if (plasma > VISIBILITY_THRESHOLD)
        {
            FrameData::Entity entity;
            entity.id = frame->GetUniqueIdForNewFrame(GetId(), 1);

            entity.click_id = 0;
            entity.pos_x = GetPosition().x;
            entity.pos_y = GetPosition().y;
            entity.vlevel = 11;
            entity.view.base_frameset.sprite_name = "icons/plasma.dmi";
            entity.view.base_frameset.state = "plasma";
            const double FULL_VISIBILITY_THRESHOLD = 100.0;
            const double visibility = (plasma * 1.0) * (MAX_TRANSPARENCY / FULL_VISIBILITY_THRESHOLD);
            entity.view.transparency = qMin(static_cast<int>(visibility), MAX_TRANSPARENCY);
            entity.dir = Dir::SOUTH;
            frame->Append(entity);
        }
    }
    MaterialObject::Represent(frame, mob);
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
