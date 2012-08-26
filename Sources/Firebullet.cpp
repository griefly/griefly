#include "Firebullet.h"
#include "Mob.h"
#include "MapClass.h"

//itemAdder<CFirebullet> adder; 

void CFirebullet::process()
{
    imageStateW = dMove;
    int count = 0;

    auto itr = map->squares[posx][posy].begin();
    while(itr != map->squares[posx][posy].end())
    {
        if(castTo<IMainItem>(**itr)) {itr++;continue;}
        (*itr++)->delThis();
        ++count;
    }
    itr = map->squares[posx][posy].begin();
    while(itr != map->squares[posx][posy].end())
    {
        CMonstr* type;
        type = castTo<CMonstr>(**itr++);
        //if(type) type->attack_by(id);
        //(*itr++)->delThis();
        //itr++;
        ++count;
    }
    if(count) delThis();
    else if(!checkPassable()){delThis();}
    else checkMove(dMove);
};

CFirebullet::CFirebullet()
{
    tickSpeed = 8;
    pixSpeed = 4;
    SetSprite("icons/spear.png");
};