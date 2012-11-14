#include "OnMapInt.h"
#include "MapClass.h"
#include "MobInt.h"

#include "MoveEffect.h"
#include "TileInt.h"

void IOnMapItem::attack_by(id_ptr_on<SmallItem> it, int force) {};

std::list<HashAmount> IOnMapItem::insertLiquid(std::list<HashAmount> r) {return r;};

bool IOnMapItem::checkMove(Dir direct)
{
    //SYSTEM_STREAM << "\nCheck move " << id <<"\n";
    if(!checkMoveTime()) return false;
    dMove = direct;
    if(!checkPassable()) return false;
    return mainMove();    
};

void IOnMapItem::move(Dir direct)
{
    switch(direct)
    {
    case D_LEFT:
        x -= pixSpeed;
        break;
    case D_RIGHT:
        x += pixSpeed;
        break;
    case D_UP:
        y -= pixSpeed;
        break;
    case D_DOWN:
        y += pixSpeed;
        break;
    }   
};

/*void IOnMapItem::mobMove()
{

    MapMaster::switchDir(x, y, mob->dMove, mob->pixSpeed, true);//Trurlu ok
};*/

bool IOnMapItem::isVisible(int x, int y)
{
    if ( x >= std::max(0, (castTo<CubeTile>(mobMaster->thisMob->GetOwner().ret_item())->posx()) - sizeHsq) &&
         x <= std::min((castTo<CubeTile>(mobMaster->thisMob->GetOwner().ret_item())->posx()) + sizeHsq, sizeHmap - 1) &&
         y >= std::max(0, (castTo<CubeTile>(mobMaster->thisMob->GetOwner().ret_item())->posy()) - sizeWsq) &&
         y <= std::min((castTo<CubeTile>(mobMaster->thisMob->GetOwner().ret_item())->posx()) + sizeWsq, sizeWmap - 1))
        return 1;
    return 0;
};

bool IOnMapItem::checkMoveTime()
{
    if(MAIN_TICK - lastMove <= tickSpeed) 
        return false;
    lastMove = static_cast<int>(MAIN_TICK);
    return true;
};

// TODO: ÒÎÐÍÀÄÎ
bool IOnMapItem::checkPassable()
{
    return owner->GetNeighbour(dMove)->IsPassable();
};

bool IOnMapItem::mainMove()
{
    auto new_owner = owner->GetNeighbour(dMove);
    if (new_owner == owner)
        return false;

    owner->RemoveItem(id);
    new_owner->AddItem(id);

    if(new_owner->IsVisibleByPlayer())
    {
        Move* eff = getEffectOf<Move>();
        eff->Init(TITLE_SIZE, dMove, pixSpeed, id, true);
        eff->Start();
    }
    return true;
};

void IOnMapItem::delThis()
{
}

IOnMapItem::IOnMapItem()
{
    lastMove = 0;
    tickSpeed = 1;
    pixSpeed = 1;
    dMove = D_UP;
    passable = true;
    transparent = true;
    burn_power = 0;
    name = "NONAMESHIT";
}