#include "Materials.h"

#include "core/objects/movable/structures/Girder.h"
#include "core/ObjectFactory.h"
#include "core/objects/turfs/Floor.h"
#include "core/objects/movable/structures/Grille.h"

using namespace kv;

Rod::Rod()
{
    SetState("rods");

    name = "Rods";
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

    name = "Metal";
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
