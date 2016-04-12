#include <assert.h>

#include "OnMapObject.h"
#include "../Map.h"
#include "Mob.h"
#include "../Game.h"
#include "Tile.h"

#include "Creator.h"
#include "../Helpers.h"

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
    Representation::Entity ent;
    ent.id = GetId();
    ent.pos_x = GetX();
    ent.pos_y = GetY();
    ent.vlevel = v_level;
    ent.view = *GetView();
    ent.dir = D_DOWN;
    GetRepresentation().AddToNewFrame(ent);
}

void IOnMapObject::ProcessPhysics()
{
    if (owner->IsStrongOwner())
    {
        return;
    }
    auto down = owner->GetNeighbour(D_ZDOWN);
    if (    down.valid() 
        && (CanPass(down->GetPassable(D_ZUP), passable_level)) 
        && (CanPass(down->GetPassable(D_ALL), passable_level)) )
    {
        owner->RemoveItem(GetId());
        down->AddItem(GetId());
    }
}
void IOnMapObject::Delete()
{
    IOnMapBase::Delete();
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
