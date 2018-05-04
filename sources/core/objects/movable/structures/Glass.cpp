#include "Glass.h"

#include "SynchronizedRandom.h"
#include "objects/mobs/Mob.h"
#include "objects/mobs/Human.h"
#include "Breakable.h"
#include "objects/movable/items/Shard.h"
#include "objects/PhysicsEngine.h"
#include "objects/movable/items/Limbs.h"

using namespace kv;

FlatGlass::FlatGlass()
{
    SetTransparency(true);

    SetTickSpeed(5);
    SetHitPoints(20);

    SetVisibleLevel(visible_level::STANDING_MOB);

    SetSprite("icons/structures.dmi");
    SetState("window");
    
    SetName("Window");
}

void FlatGlass::AfterWorldCreation()
{
    Structure::AfterWorldCreation();

    SetPassable(Dir::NORTH, passable::FULL);
    SetPassable(Dir::SOUTH, passable::FULL);
    SetPassable(Dir::WEST, passable::FULL);
    SetPassable(Dir::EAST, passable::FULL);

    SetPassable(GetDir(), passable::EMPTY);
}

void FlatGlass::ApplyForce(const Vector& force, ForceSource source)
{
    const int max = std::max(std::max(force.x, force.y), force.z);
    const int BORDERLINE = 30 * FORCE_UNIT;
    if (max > BORDERLINE)
    {
        Break();
        return;
    }
    Breakable::ApplyForce(force, source);
}

void FlatGlass::Bump(const Vector& vector, IdPtr<Movable> item)
{
    if (IdPtr<Mob> mob = item)
    {
        if (item->GetDir() != GetDir())
        {
            if (!CanPass(GetOwner()->GetPassable(Dir::ALL), GetPassableLevel()) || IsAnchored())
            {
                return;
            }

            Rotate(item->GetDir());
            return;
        }
        Movable::Bump(vector, item);
        return;
    }
    Breakable::Bump(vector, item);
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
    Create<Shard>(GetOwner());
    Delete();
}

void FlatGlass::PlayOnHitSound()
{
    PlaySoundIfVisible("Glasshit.wav");
}

void FlatGlass::AttackBy(IdPtr<Item> item)
{
    if (IdPtr<Hand> hand = item)
    {
        if (IdPtr<Human> human = item->GetOwner())
        {
            PostVisible(
                QString("%1 knocks on window.").arg(human->GetName()), GetPosition());
        }
        PlaySoundIfVisible("Glasshit.wav");
        return;
    }
    Breakable::AttackBy(item);
}

ReinforcedFlatGlass::ReinforcedFlatGlass()
{
    SetState("rwindow");
    SetName("Reinforced window");
    SetHitPoints(100);
}
