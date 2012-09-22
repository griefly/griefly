#include "OnMapInt.h"
#include "MapClass.h"
#include "MobInt.h"

#include "MoveEffect.h"

void IOnMapItem::attack_by(id_ptr_on<SmallItem> it, int force) {};

std::list<HashAmount> IOnMapItem::insertLiquid(std::list<HashAmount> r) {return r;};

/*void IOnMapItem::processMove()
{
    if(isMove)
    {
        //SYSTEM_STREAM << "\nMove " << id << " " << this << " " << isMove << "\n";
        way -= pixSpeed;
        move(dMove);
        if(way <= 0)
        {
            SYSTEM_STREAM << "\nSet ISMOVE false " << id << "\n";
            isMove = false;
            way = 0;
        }
    }
};*/

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
    if ( x >= max(0, (mobMaster->thisMob->posx) - sizeHsq) &&
         x <= min((mobMaster->thisMob->posx) + sizeHsq, sizeHmap - 1) &&
         y >= max(0, (mobMaster->thisMob->posy) - sizeWsq) &&
         y <= min((mobMaster->thisMob->posy) + sizeWsq, sizeWmap - 1))
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

bool IOnMapItem::checkPassable()
{
    Dir direct = dMove;
    if(!MapMaster::checkOutBorder(posx, posy, direct)) return false;
    int locx = posx;
    int locy = posy;
    MapMaster::switchDir(locx, locy, direct);//trurly

    //SYSTEM_STREAM << "Begin use is passable\n";
    if(map->isPassable(locx, locy, level))
        return true;
    //SYSTEM_STREAM << "Bad end\n";
    return false;
};

bool IOnMapItem::mainMove()
{
    //SYSTEM_STREAM << "Main move " << id << "\n";
    Dir direct = dMove;
    auto itr = map->squares[posx][posy].begin();
    while((itr != map->squares[posx][posy].end()) && !((**itr) == this)) ++itr;
    if(itr == map->squares[posx][posy].end())
    {
        SYSTEM_STREAM << "Del irreal object " << posx << " " << posy << std::endl;
        return false;
    }
    map->squares[posx][posy].erase(itr);
    int oldposy = posy;
    int oldposx = posx;
    MapMaster::switchDir(posx, posy, direct);//trurly
    id_ptr_on<IOnMapItem> thisid;
    thisid = id;
    if(!passable && !level) 
        checkForEachLevel(oldposx, oldposy);
    checkSelfLevel();
    MapMaster::switchDir(x, y, direct, TITLE_SIZE, false);
    map->addItemOnMap(thisid);
    if(isVisible(posx, posy))
    {
        Move* eff = getEffectOf<Move>();
        eff->Init(TITLE_SIZE, direct, pixSpeed, id, true);
        eff->Start();
    }
    return true;
};

void IOnMapItem::checkSelfLevel()
{
    if(!level) return;
    if(checkLevel(posx, posy)) return;
    level = 0;
    return;
}

void IOnMapItem::checkForEachLevel(int posx, int posy)
{
    if(checkLevel(posx, posy)) return;
    setEachLevel(posx, posy, 0);
}

void IOnMapItem::setEachLevel(int posx, int posy, bool level)
{
    auto itr = map->squares[posx][posy].begin();
    while(itr != map->squares[posx][posy].end())
        (*itr++)->level = level;
    return;
}

bool IOnMapItem::checkLevel(int posx, int posy)
{
    auto itr = map->squares[posx][posy].begin();
    while(itr != map->squares[posx][posy].end())
        if(!(*itr)->level && !(*itr)->passable) return 1;
        else ++itr;
    return 0;
}

void IOnMapItem::delThis()
{
    auto itr = map->squares[posx][posy].begin();
    while((itr != map->squares[posx][posy].end()) && !((**itr) == this)) itr++;
    if(itr == map->squares[posx][posy].end())
    {
        SYSTEM_STREAM << "Del irreal object " << posx << " " << posy << std::endl;
        return;
    }
    map->squares[posx][posy].erase(itr);
    IMainItem::delThis();
}

void IOnMapItem::LoadInMap()
{
    id_ptr_on<IOnMapItem> pushval;
    pushval = id;
    map->addItemOnMap(pushval);
}

IOnMapItem::IOnMapItem()
{
    lastMove = 0;
    tickSpeed = 1;
    pixSpeed = 1;
    dMove = D_UP;
    passable = true;
    transparent = true;
    posx = 0;//local coord, 1..n, N ý n
    posy = 0;
    level = 1;//0 - 1
    burn_power = 0;
    name = "NONAMESHIT";
}