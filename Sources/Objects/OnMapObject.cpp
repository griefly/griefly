#include <assert.h>

#include "OnMapObject.h"
#include "Map.h"
#include "Mob.h"
#include "Manager.h"
#include "Tile.h"

#include "Creator.h"
#include "MobPosition.h"
#include "helpers.h"

std::list<HashAmount> IOnMapObject::insertLiquid(std::list<HashAmount> r) {return r;}

void IOnMapObject::SetSprite(const std::string& name)
{
    view_.SetSprite(name);
}

void IOnMapObject::SetState(const std::string& name)
{
    view_.SetState(name);
}

void IOnMapObject::Represent()
{ 
    /*DrawMain(0,
            GetDrawX() + mob_position::get_shift_x(),
            GetDrawY() + mob_position::get_shift_y());*/
};

void IOnMapObject::processPhysics()
{
    if (owner->IsStrongOwner())
        return;
    auto down = owner->GetNeighbour(D_ZDOWN);
    if (    down.valid() 
        && (CanPass(down->GetPassable(D_ZUP), passable_level)) 
        && (CanPass(down->GetPassable(D_ALL), passable_level)) )
    {
        owner->RemoveItem(GetId());
        down->AddItem(GetId());
    }
}
void IOnMapObject::delThis()
{
    IOnMapBase::delThis();
}

IOnMapObject::IOnMapObject(size_t id) : IOnMapBase(id)
{
    v_level = 0;
    passable_all = Passable::FULL;
    passable_up = Passable::FULL;
    passable_down = Passable::FULL;
    passable_left = Passable::FULL;
    passable_right = Passable::FULL;

    passable_level = Passable::FULL;

    transparent = true;
    name = "nameless";
}
