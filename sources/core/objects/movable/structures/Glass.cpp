#include "Glass.h"

#include "core/SynchronizedRandom.h"
#include "core/Helpers.h"
#include "core/objects/mobs/Mob.h"
#include "Breakable.h"
#include "core/objects/movable/items/Shard.h"

using namespace kv;

FlatGlass::FlatGlass()
{
    transparent = true;

    tick_speed_ = 5;
    SetHitPoints(20);

    v_level = 9;

    SetSprite("icons/structures.dmi");
    SetState("window");
    
    name = "Window";
}

void FlatGlass::AfterWorldCreation()
{
    Structure::AfterWorldCreation();

    SetPassable(Dir::UP, passable::FULL);
    SetPassable(Dir::DOWN, passable::FULL);
    SetPassable(Dir::LEFT, passable::FULL);
    SetPassable(Dir::RIGHT, passable::FULL);

    SetPassable(GetDir(), passable::EMPTY);
}

void FlatGlass::Bump(IdPtr<Movable> item)
{
    if (IdPtr<Mob> mob = item)
    {
        if (item->GetDir() != GetDir())
        {
            if (!CanPass(GetOwner()->GetPassable(Dir::ALL), passable_level) || anchored_)
            {
                return;
            }

            Rotate(item->GetDir());
        }
        Movable::Bump(item);
        return;
    }
    Breakable::Bump(item);
}

bool FlatGlass::Rotate(Dir dir)
{
    SetPassable(GetDir(), passable::FULL);
    Structure::Rotate(dir);
    SetPassable(GetDir(), passable::EMPTY);
    return true;
}

void FlatGlass::Break()
{
    QString sound = QString("Glassbr%1.wav").arg(GenerateRandom() % 3 + 1);
    PlaySoundIfVisible(sound);
    Create<Item>(Shard::GetTypeStatic(), GetOwner());
    Delete();
}

void FlatGlass::PlayOnHitSound()
{
    PlaySoundIfVisible("Glasshit.wav");
}

void FlatGlass::AttackBy(IdPtr<Item> item)
{
    if (!item.IsValid())
    {
/*      GetGame().GetChat().PostSimpleText(
        name + " knocks on window. " +, GetRoot().Id());*/
        PlaySoundIfVisible("Glasshit.wav");
        return;
    }
    Breakable::AttackBy(item);
}

ReinforcedFlatGlass::ReinforcedFlatGlass()
{
    SetState("rwindow");
    name = "Reinforced window";
    SetHitPoints(100);
}
