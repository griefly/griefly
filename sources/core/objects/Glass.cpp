#include "Glass.h"

#include "../SyncRandom.h"
#include "../Helpers.h"
#include "Mob.h"
#include "Breakable.h"
#include "Shard.h"

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

    SetPassable(Dir::UP, Passable::FULL);
    SetPassable(Dir::DOWN, Passable::FULL);
    SetPassable(Dir::LEFT, Passable::FULL);
    SetPassable(Dir::RIGHT, Passable::FULL);

    SetPassable(GetDir(), Passable::EMPTY);
}

void FlatGlass::Bump(IdPtr<IMovable> item)
{
    if (IdPtr<IMob> mob = item)
    {
        if (item->GetDir() != GetDir())
        {
            if (!CanPass(owner->GetPassable(Dir::ALL), passable_level) || anchored_)
            {
                return;
            }

            Rotate(item->GetDir());
        }
        IMovable::Bump(item);
        return;
    }
    Breakable::Bump(item);
}

bool FlatGlass::Rotate(Dir dir)
{
    SetPassable(GetDir(), Passable::FULL);
    Structure::Rotate(dir);
    SetPassable(GetDir(), Passable::EMPTY);
    return true;
}

void FlatGlass::Break()
{
    QString sound = QString("Glassbr%1.wav").arg(GetRand() % 3 + 1);
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
