#include "Materials.h"

#include "Girder.h"
#include "../ObjectFactory.h"
#include "../Game.h"
#include "Floor.h"
#include "Grille.h"

Rod::Rod(size_t id) : Item(id)
{
    SetState("rods");

    name = "Rods";
}

void Rod::AttackBy(id_ptr_on<Item> item)
{
    if (item.ret_id() == GetId()) // Attack self
    {
        game_->GetFactory().Create<IOnMapObject>(Grille::T_ITEM_S(), GetTurf());
        Delete();
    }
}

Metal::Metal(size_t id) : Item(id)
{
    SetState("sheet-metal");

    name = "Metal";
}

void Metal::AttackBy(id_ptr_on<Item> item)
{
    if (item.ret_id() == GetId()) // Attack self
    {
        if (id_ptr_on<Floor> floor = GetTurf())
        {
            game_->GetFactory().Create<IOnMapObject>(Girder::T_ITEM_S(), GetTurf());
            Delete();
        }
    }
}
