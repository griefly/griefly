#include "Grille.h"

#include "Mob.h"
#include "Item.h"
#include "representation/Sound.h"

#include "../ObjectFactory.h"
#include "../Game.h"
#include "Materials.h"

Grille::Grille(quint32 id) : Breakable(id)
{
    transparent = true;
    SetPassable(Dir::ALL, Passable::AIR);

    tick_speed_ = 5;
    SetHitPoints(15);

    v_level = 8;

    SetSprite("icons/structures.dmi");
    SetState("grille");

    name = "Grille";

    cutted_ = false;
}

void Grille::AttackBy(IdPtr<Item> item)
{
    if (IdPtr<Wirecutters> w = item)
    {
        PlaySoundIfVisible("Wirecutter.wav");
        if (!cutted_)
        {
            SetState("brokengrille");
            SetPassable(Dir::ALL, Passable::FULL);
            cutted_ = true;
            Create<IOnMapObject>(Rod::GetTypeStatic(), GetOwner());
        }
        else
        {
            Create<IOnMapObject>(Rod::GetTypeStatic(), GetOwner());
            Delete();
        }
        return;
    }
    if (!item.IsValid())
    {
        Hit(2);
        return;
    }
    Breakable::AttackBy(item);
}

void Grille::Break()
{
    if (!cutted_)
    {
        SetState("brokengrille");
        SetPassable(Dir::ALL, Passable::FULL);
        cutted_ = true;
        Create<IOnMapObject>(Rod::GetTypeStatic(), GetOwner());
        SetHitPoints(7);
    }
    else
    {
        Create<IOnMapObject>(Rod::GetTypeStatic(), GetOwner());
        Delete();
    }
}

void Grille::PlayOnHitSound()
{
    PlaySoundIfVisible("grillehit.wav");
}
