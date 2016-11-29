#include "Floor.h"

#include "representation/Sound.h"

#include "Item.h"
#include "FloorTile.h"
#include "Pipes.h"

#include "../ObjectFactory.h"
#include "../Game.h"

Floor::Floor(quint32 id) : ITurf(id)
{
    transparent = true;
    SetPassable(D_ALL, Passable::FULL);

    SetSprite("icons/floors.dmi");
    SetState("floor");

    name = "Floor";
    
    FloorType = "floor";
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
            switch(qHash(FloorType))
            {
                case 2179427974:
                    Create<Item>(FloorTile::T_ITEM_S(), GetOwner());
                    break;
                case 3691820876:
                    Create<Item>(WhiteFloorTile::T_ITEM_S(), GetOwner());
                    break;
            }
            PlaySoundIfVisible("Crowbar.ogg", owner.Id());
        }
    }
    else if (IdPtr<FloorTile> ftile = item)
    {
        if (open_)
        {
            SetOpen(false);
            ftile->Delete();
            FloorType = ftile->Type;
            PlaySoundIfVisible("Deconstruct.ogg", owner.Id());
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
        SetState(FloorType);
        v_level = 2;
        if (auto vent = owner->GetItem<Vent>())
        {
            vent->SetHidden(true);
        }
    }
    GetView()->RemoveOverlays();
    //qDebug() << "End setopen";
}

Plating::Plating(quint32 id) : Floor(id)
{
    open_ = true;
    // For map editor
    SetState("plating");
}
WhiteFloor::WhiteFloor(quint32 id) : Floor(id)
{
    open_ = false;
    // For map editor
    SetState("whitebot");
    FloorType = "whitebot";
}
