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

    base_state_ = "metal";

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
    CheckNeighborhood(Dir::ALL);
    UpdateState();
}

void MetalWall::Delete()
{
    NotifyNeighborhood(Dir::ALL);
    MaterialObject::Delete();
}

void MetalWall::CheckNeighborhood(const Dir dir)
{
    if (dir == Dir::ALL)
    {
        for (const auto& x : WALL_PROCESSING_DIRS)
        {
            CheckNeighborhood(x);
        }
        return;
    }

    if (IdPtr<MetalWall> wall = GetNeighbour(dir)->GetTurf())
    {
        SetBitByDirection(dir, true);

        wall->SetBitByDirection(RevertDir(dir), true);
        wall->UpdateState();
    }
    else
    {
        SetBitByDirection(dir, false);
    }
}

void MetalWall::UpdateState()
{
    SetState(base_state_ + QString::number(current_state_.to_ulong()));
}

void MetalWall::NotifyNeighborhood(const Dir dir)
{
    if (dir == Dir::ALL)
    {
        for (const auto& x : WALL_PROCESSING_DIRS)
        {
            NotifyNeighborhood(x);
        }
        return;
    }

    if (IdPtr<MetalWall> wall = GetNeighbour(dir)->GetTurf())
    {
        wall->SetBitByDirection(RevertDir(dir), false);
        wall->UpdateState();
    }
}

ReinforcedWall::ReinforcedWall()
{
    SetTransparency(false);
    SetPassable(Dir::ALL, passable::EMPTY);

    SetVisibleLevel(visible_level::HIGH_TURF);

    SetSprite("icons/walls.dmi");

    SetName("Reinforced wall");

    base_state_ = "rwall";

    SetState("rwall0");
}

void ReinforcedWall::AttackBy(IdPtr<Item> item)
{
    // Nothing
}

void MetalWall::SetBitByDirection(const Dir dir, const bool value)
{
    int bit;

    switch (dir)
    {
    case Dir::NORTH:
        bit = 0;
        break;
    case Dir::SOUTH:
        bit = 1;
        break;
    case Dir::EAST:
        bit = 2;
        break;
    case Dir::WEST:
        bit = 3;
        break;
    default:
        return;
    }

    current_state_.set(bit, value);
}
