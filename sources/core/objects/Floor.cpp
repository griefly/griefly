#include "Floor.h"

#include "representation/Sound.h"

#include "Item.h"
#include "FloorTile.h"
#include "Pipes.h"

#include "../ObjectFactory.h"
#include "../Game.h"

using namespace kv;

Floor::Floor()
{
    transparent = true;
    SetPassable(Dir::ALL, Passable::FULL);

    SetSprite("icons/floors.dmi");
    SetState("floor");

    name = "Floor";

    floor_type_ = "floor";
    open_ = false;
    bloody = false;
}

void Floor::AfterWorldCreation()
{
    ITurf::AfterWorldCreation();
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
        v_level = 0;
        if (auto vent = owner->GetItem<Vent>())
        {
            vent->SetHidden(false);
        }
    }
    else
    {
        SetState(floor_type_);
        v_level = 2;
        if (auto vent = owner->GetItem<Vent>())
        {
            vent->SetHidden(true);
        }
    }
    GetView()->RemoveOverlays();
    //qDebug() << "End setopen";
}

Plating::Plating()
{
    open_ = true;
    // For map editor
    SetState("plating");
    floor_type_ = "floor";
}


