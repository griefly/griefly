#include "Materials.h"

#include "Girder.h"
#include "../ObjectFactory.h"
#include "../Game.h"
#include "Floor.h"
#include "Grille.h"

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
        Create<IOnMapObject>(Grille::GetTypeStatic(), GetTurf());
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
            Create<IOnMapObject>(Girder::GetTypeStatic(), GetTurf());
            Delete();
        }
    }
}
