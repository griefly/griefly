#include "Floor.h"

#include "objects/movable/items/Item.h"
#include "objects/movable/items/FloorTile.h"
#include "objects/movable/Pipes.h"

#include "ObjectFactory.h"

using namespace kv;

Floor::Floor()
{
    SetTransparency(true);
    SetPassable(Dir::ALL, passable::FULL);

    SetSprite("icons/floors.dmi");
    SetState("floor");

    SetName("Floor");

    floor_type_ = "floor";
    open_ = false;
    bloody = false;
}

void Floor::AfterWorldCreation()
{
    Turf::AfterWorldCreation();
    SetOpen(open_);
}

void Floor::AttackBy(IdPtr<Item> item)
{
    if (IdPtr<Crowbar> c = item)
    {
        if (!open_)
        {
            SetOpen(true);
            Create<Item>(FloorTile::GetTypeStatic(), GetOwner());
            PlaySoundIfVisible("Crowbar.wav");
        }
    }
    else if (IdPtr<FloorTile> ftile = item)
    {
        if (open_)
        {
            SetOpen(false);
            ftile->Delete();
            PlaySoundIfVisible("Deconstruct.wav");
        }
    }
}

void Floor::SetOpen(bool o)
{
    //qDebug() << "Start set open " << owner->GetId();
    open_ = o;
    if (open_)
    {
        SetState("plating");
        SetVisibleLevel(0);
        if (auto vent = GetOwner()->GetItem<Vent>())
        {
            vent->SetHidden(false);
        }
    }
    else
    {
        SetState(floor_type_);
        SetVisibleLevel(2);
        if (auto vent = GetOwner()->GetItem<Vent>())
        {
            vent->SetHidden(true);
        }
    }
    GetView().RemoveOverlays();
    //qDebug() << "End setopen";
}

void Floor::ApplyFire(int intensity)
{
    Q_UNUSED(intensity);

    if (GenerateRandom() % 4)
    {
        return;
    }

    if (open_)
    {
        SetState("panelscorched");
    }
    else
    {
        if (GenerateRandom() % 2)
        {
            SetState("floorscorched1");
        }
        else
        {
            SetState("floorscorched2");
        }
    }
}

Plating::Plating()
{
    open_ = true;
    // For map editor
    SetState("plating");
    floor_type_ = "floor";
}


