#include "MobInt.h"
#include "Mob.h"
#include "MapClass.h"
#include "Turf.h"

IMob::IMob()
{
    thisMobControl = false;
    onMobControl = false;
    SetFreq(1);
    SetSprite("icons/ork.png");        
}

void IMob::cautOverMind()
{
    if(onMobControl)
    {
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
    // else if (msg.text == "SDLK_j")
    //    level = 1;
    else if (msg.text == "SDLK_1")
    {
        h.hash = hash("petrol");
        h.amount = 1;
        hloc.push_back(h);
        // map->splashLiquid(hloc, posx, posy);
    }
    else if(msg.text == "SDLK_2")
    {
        h.hash = hash("honey");
        h.amount = 1;
        hloc.push_back(h);
        // map->splashLiquid(hloc, posx, posy);
    }
    else if(msg.text == "SDLK_3")
    {
        h.hash = hash("water");
        h.amount = 3;
        hloc.push_back(h);
        // map->splashLiquid(hloc, posx, posy);
    }
    else if(msg.text == "SDLK_f")
    {
        /*auto whoa = owner->getItemOnly<CGround>();
        if(whoa != map->squares[posx][posy].end())
        {
            (*whoa)->delThis();
            fabric->newItemOnMap<Pit>(hash("pit"), posx, posy);
        }*/
    }
}

void IMob::processPhysics()
{
    IMessageReceiver::processPhysics();
    if(thisMobControl) 
        GetManager()->UpdateVisible();
}

bool IMob::checkMove(Dir direct)
{
    if(IOnMapItem::checkMove(direct))
    {        
        
        if(thisMobControl)
        {            
            //SYSTEM_STREAM << "Function IMob::checkMove called: onMobControl == true\n";
            GetManager()->checkMove(direct);
            GetManager()->UpdateVisible();
        }
        return true;
    }
    return false;
}