#include "MetalWall.h"

#include "objects/movable/items/Weldingtool.h"
#include "objects/movable/structures/Girder.h"
#include "Floor.h"
#include "objects/movable/items/Materials.h"

#include "ObjectFactory.h"

using namespace kv;

MetalWall::MetalWall()
{
    SetTransparency(false);
    SetPassable(Dir::ALL, passable::EMPTY);

    SetVisibleLevel(visible_level::HIGH_TURF);

    SetSprite("icons/walls.dmi");

    SetName("Metal wall");

    default_state_="metal";
    current_state_=0;

    SetState("metal0");
}

void MetalWall::AttackBy(IdPtr<Item> item)
{
    if (IdPtr<Weldingtool> wtool = item)
    {
        if (wtool->IsWorking())
        {
            PlaySoundIfVisible("Welder.wav");
            Create<Girder>(GetOwner());
            Create<Metal>(GetOwner());
            Create<Plating>(GetOwner());
            Delete();
        }
    }
}

void MetalWall::AfterWorldCreation()
{
    UpdateState(Dir::ALL);
}

void MetalWall::Delete()
{
    UpdateNeighborhoodState();
    MaterialObject::Delete();
}

void MetalWall::CheckNeighborhood(Dir dir)
{
    if (dir == Dir::ALL)
    {
        for(auto x : WALL_PROCESSING_DIRS) CheckNeighborhood(x);
        return;
    }

    int bit;

    switch(dir)
    {
        case Dir::NORTH: bit=0; break;
        case Dir::SOUTH: bit=1; break;
        case Dir::EAST:  bit=2; break;
        case Dir::WEST:  bit=3; break;
        default: return;
    }

    if (IdPtr<MetalWall> wall = GetNeighbour(dir)->GetTurf())
    {
        current_state_.set(bit, true);
        return;
    }
    current_state_.set(bit, false);
}

void MetalWall::UpdateState(Dir dir)
{
    CheckNeighborhood(dir);
    if (dir == Dir::ALL)
    {
        UpdateNeighborhoodState();
    }
    SetState(default_state_ + QString::number(current_state_.to_ulong()));
}

void MetalWall::UpdateNeighborhoodState(Dir dir)
{
    if (dir == Dir::ALL)
    {
        for(auto x : WALL_PROCESSING_DIRS) UpdateNeighborhoodState(x);
        return;
    }

    if (IdPtr<MetalWall> wall = GetNeighbour(dir)->GetTurf())
    {
        wall->UpdateState(RevertDir(dir));
    }
}

ReinforcedWall::ReinforcedWall()
{
    SetTransparency(false);
    SetPassable(Dir::ALL, passable::EMPTY);

    SetVisibleLevel(visible_level::HIGH_TURF);

    SetSprite("icons/walls.dmi");

    SetName("Reinforced wall");

    default_state_="rwall";
    current_state_=0;

    SetState("rwall0");
}

void ReinforcedWall::AfterWorldCreation()
{
    UpdateState(Dir::ALL);
}

void ReinforcedWall::Delete()
{
    UpdateNeighborhoodState();
    MaterialObject::Delete();
}

void ReinforcedWall::AttackBy(IdPtr<Item> item)
{
    //Nothing
}
