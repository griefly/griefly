#include "MobInt.h"
#include "Mob.h"
#include "MapClass.h"
#include "Turf.h"

IMob::IMob()
{
    thisMobControl = false;
    onMobControl = false;
    SetSprite("icons/ork.png");        
}

void IMob::cautOverMind()
{
    if(onMobControl)
    {
        /*initItem init;
        init.dMove = D_UP;
        init.imageStateH = 0;
        init.imageStateW = 0;
        init.posx = posx;
        init.posy = posy;
        init.x = x;
        init.y = y;
        init.type = hash("meat");*/ 
        //mobMaster->changeMob(map->newItem(init));
    }
};

void IMob::processGUI()
{
}

void IMob::delThis()
{
    DeinitGUI();
    cautOverMind();
    IOnMapItem::delThis();
}

void IMob::processGUImsg(const Message& msg)
{
    int i;
    int& j = i;
    int& u = j;
    HashAmount h;
    std::list<HashAmount> hloc;
    if (msg.text == "SDLK_UP")
        checkMove(D_UP);
    else if (msg.text == "SDLK_DOWN")
        checkMove(D_DOWN);
    else if (msg.text == "SDLK_LEFT")
        checkMove(D_LEFT);
    else if (msg.text == "SDLK_RIGHT")
        checkMove(D_RIGHT);
    else if (msg.text == "SDLK_j")
        level = 1;
    else if (msg.text == "SDLK_1")
    {
        h.hash = hash("petrol");
        h.amount = 1;
        hloc.push_back(h);
        map->splashLiquid(hloc, posx, posy);
    }
    else if(msg.text == "SDLK_2")
    {
        h.hash = hash("honey");
        h.amount = 1;
        hloc.push_back(h);
        map->splashLiquid(hloc, posx, posy);
    }
    else if(msg.text == "SDLK_3")
    {
        h.hash = hash("water");
        h.amount = 3;
        hloc.push_back(h);
        map->splashLiquid(hloc, posx, posy);
    }
    else if(msg.text == "SDLK_f")
    {
        auto whoa = map->getItemOnly<CGround>(posx, posy);
        if(whoa != map->squares[posx][posy].end())
        {
            (*whoa)->delThis();
            fabric->newItemOnMap<Pit>(hash("pit"), posx, posy);
        }
    }
}

bool IMob::checkMove(Dir direct)
{
    if(IOnMapItem::checkMove(direct))
    {        
        
        if(thisMobControl)
        {            
            //SYSTEM_STREAM << "Function IMob::checkMove called: onMobControl == true\n";
            mobMaster->checkMove(direct);
            mobMaster->visiblePoint->clear();
            mobMaster->visiblePoint = map->losf.calculateVisisble(mobMaster->visiblePoint, posx, posy, level);
        }
        return true;
    }
    return false;
}