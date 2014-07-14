#include "MonstrClass.h"
#include "MapClass.h"
#include "Ork.h"
#include "sync_random.h"
#include "CSmallItem.h"

void CMonstr::aaMind()
{

   auto ptr = owner->GetItem<CWeed>();
   if (ptr.valid())
       ptr.valid(); // Do smth

   checkMove((Dir)(get_rand() % 4));
};

CMonstr::CMonstr()
{
    FindTargetTime = SDL_GetTicks() + get_rand() % 100;
    SetPassable(D_ALL, true);
    tickSpeed = 8;
    pixSpeed = 4;
    SetSprite("icons/deer.png");
    lastMove = get_rand() % tickSpeed;
    moveToTarget = false;
}

void CMonstr::live()
{
};

void CMonstr::findTarget()
{

    /*for(int lx = max(0, posx - sizeHsq); lx <= min(posx + sizeHsq, sizeHmap - 1); lx++)
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
        }*/

    /*findRequest.id = 0;
    idpoint p;
    p.find_hash = hash("alive");
    p.id = id;
    p.posx = posx;
    p.posy = posy;
    SYSTEM_STREAM << "Send path request\n";
    map->findRequests.push_back(p);*/
};