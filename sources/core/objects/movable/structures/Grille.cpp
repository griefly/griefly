#include "Grille.h"

#include "core/objects/mobs/Mob.h"
#include "core/objects/movable/items/Item.h"

#include "core/ObjectFactory.h"
#include "core/objects/movable/items/Materials.h"

using namespace kv;

Grille::Grille()
{
    transparent = true;
    SetPassable(Dir::ALL, passable::AIR);

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
            SetPassable(Dir::ALL, passable::FULL);
            cutted_ = true;
            Create<MaterialObject>(Rod::GetTypeStatic(), GetOwner());
        }
        else
        {
            Create<MaterialObject>(Rod::GetTypeStatic(), GetOwner());
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
        SetPassable(Dir::ALL, passable::FULL);
        cutted_ = true;
        Create<MaterialObject>(Rod::GetTypeStatic(), GetOwner());
        SetHitPoints(7);
    }
    else
    {
        Create<MaterialObject>(Rod::GetTypeStatic(), GetOwner());
        Delete();
    }
}

void Grille::PlayOnHitSound()
{
    PlaySoundIfVisible("grillehit.wav");
}
