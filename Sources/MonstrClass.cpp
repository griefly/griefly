//#include "MonstrClass.h"
#include "MapClass.h"
#include "Ork.h"
#include "sync_random.h"

//itemAdder<CMonstr> adder; 

void CMonstr::aaMind()
{

    auto itr = map->squares[posx][posy].begin();

    while(itr != map->squares[posx][posy].end())
    {

        if(castTo<CWeed>(**itr))
        {
            dmg -= 5;

            //(*itr++)->delThis(itr);

        }
        itr++;
    }

   /*if(inWay)
   {
       if(!isMove && (MAIN_TICK - lastMove > tickSpeed))
       {
           if(path.begin() == path.end())
           {
               inWay = false;
               return;
           }
           checkMove(*path.begin());
           path.pop_front();
       }
       return;
   }*/
   //static int FindTargetTime = SDL_GetTicks();
   checkMove((Dir)(get_rand() % 4));
   /*if(SDL_GetTicks() - FindTargetTime > 50)
   {
       FindTargetTime = SDL_GetTicks();
       findTarget();
       inWay = true;
   }*/
};

CMonstr::CMonstr()
{
    level = 1;
    FindTargetTime = SDL_GetTicks() + get_rand() % 100;
    passable = false;
    tickSpeed = 8;
    pixSpeed = 4;
    SetSprite("icons/deer.png");
    lastMove = get_rand() % tickSpeed;
    moveToTarget = false;
}

void CMonstr::live()
{
    if(dmg <= 0)
    {
        /*initItem init;
        init.dMove = D_UP;
        init.imageStateH = 0;
        init.imageStateW = 0;
        init.posx = posx;
        init.posy = posy;
        init.x = (posx - IMainItem::mob->posx + beginMobPosX) * TITLE_SIZE;
        init.y = (posy - IMainItem::mob->posy + beginMobPosY) * TITLE_SIZE;
        //init.type = hash("meat");
        //map->newItem(init);
        delThis();*/
    }
};

void CMonstr::findTarget()
{

    for(int lx = max(0, posx - sizeHsq); lx <= min(posx + sizeHsq, sizeHmap - 1); lx++)
        {
            for(int ly = max(0, posy - sizeWsq); ly <= min(posy + sizeWsq, sizeWmap - 1); ly++)
            {
                auto itr = map->squares[lx][ly].begin();
                while(itr != map->squares[lx][ly].end())
                {   
                    if(castTo<CAliveMob>(**itr) && **itr != this)
                    {
                        //SYSTEM_STREAM << "I FIND YOU\n";
//                        path = map->pathf.calculatePath(posx, posy, lx, ly, level);
                        //SYSTEM_STREAM << "END FIND PATH\n";
                        //sendPathRequest = true;
                        return;
                    };
                    ++itr;
                }
            }
        }

    /*findRequest.id = 0;
    idpoint p;
    p.find_hash = hash("alive");
    p.id = id;
    p.posx = posx;
    p.posy = posy;
    SYSTEM_STREAM << "Send path request\n";
    map->findRequests.push_back(p);*/
};