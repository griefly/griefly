#include "SpaceTurf.h"

#include "SynchronizedRandom.h"

#include "objects/movable/items/Item.h"
#include "objects/movable/items/Materials.h"
#include "objects/movable/structures/Lattice.h"
#include "ObjectFactory.h"

using namespace kv;

Space::Space()
{
    SetAtmosState(atmos::SPACE);

    SetPassable(Dir::ALL, passable::FULL);
    SetTransparency(true);
    
    SetFriction(0);

    SetSprite("icons/space.dmi");
    SetState("10");
    SetName("Space");
}

void Space::AfterWorldCreation()
{
    quint32 value = GenerateRandom() % 25 + 1;
    SetState(QString::number(value));
}

void Space::AttackBy(IdPtr<Item> item)
{
    if (IdPtr<Rod> rod = item)
    {
        rod->Delete();
        Create<MaterialObject>(Lattice::GetTypeStatic(), GetOwner());
    }
}
