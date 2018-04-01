#include "Materials.h"

#include "objects/movable/structures/Girder.h"
#include "ObjectFactory.h"
#include "objects/turfs/Floor.h"
#include "objects/movable/structures/Grille.h"

using namespace kv;

Rod::Rod()
{
    SetState("rods");

    SetName("Rods");
}

void Rod::AttackBy(IdPtr<Item> item)
{
    if (item.Id() == GetId()) // Attack self
    {
        Create<MaterialObject>(Grille::GetTypeStatic(), GetTurf());
        Delete();
    }
}

Metal::Metal()
{
    SetState("sheet-metal");

    SetName("Metal");
}

void Metal::AttackBy(IdPtr<Item> item)
{
    if (item.Id() == GetId()) // Attack self
    {
        if (IdPtr<Floor> floor = GetTurf())
        {
            Create<MaterialObject>(Girder::GetTypeStatic(), GetTurf());
            Delete();
        }
    }
}
