#include <assert.h>

#include "OnMapInt.h"
#include "MapClass.h"
#include "MobInt.h"
#include "Mob.h"
#include "MoveEffect.h"
#include "TileInt.h"

#include "Creator.h"
#include "mob_position.h"
#include "helpers.h"

std::list<HashAmount> IOnMapObject::insertLiquid(std::list<HashAmount> r) {return r;};

void IOnMapObject::SetSprite(const std::string& name)
{
    view_.SetSprite(name);
};


const GLSprite* IOnMapObject::GetSprite()
{
    return view_.GetSprite();
}

void IOnMapObject::SetState(const std::string& name)
{
    view_.SetState(name);
}

const ImageMetadata::SpriteMetadata* IOnMapObject::GetMetadata()
{  
    return view_.GetMetadata();
}

void IOnMapObject::DrawMain(int shift, int x, int y)
{
    view_.Draw(shift, x, y);
}

void IOnMapObject::processImage(DrawType type)
{ 
    DrawMain(0, 
            GetDrawX() + mob_position::get_shift_x(),
            GetDrawY() + mob_position::get_shift_y());
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

bool IOnMapObject::IsTransp(int x, int y)
{
    return view_.IsTransp(x, y, 0);
}
bool IOnMapObject::isVisible(int x, int y)
{
    if ( x >= std::max(0, GetMob()->GetX() - sizeHsq) &&
         x <= std::min(GetMob()->GetX() + sizeHsq, GetMapMaster()->GetMapH() - 1) &&
         y >= std::max(0, GetMob()->GetY() - sizeWsq) &&
         y <= std::min(GetMob()->GetY() + sizeWsq, GetMapMaster()->GetMapW() - 1))
        return 1;
    return 0;
};

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
    burn_power = 0;
    name = "NONAMESHIT";
}
