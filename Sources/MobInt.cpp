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
    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    //static ApproxGLImage* todraw = new ApproxGLImage(map->aSpr.returnSpr("icons/ork.png")->GetSDLSprite()->frames[0]);
    //IMainItem::map->screen->Draw(&ApproxGLImage(mobMaster->sFPS), 0, 0, 50, 30);//SDL_BlitSurface(mobMaster->sFPS, NULL, mobMaster->screen, &rect);

    /*static int locTime = 0;
    static SDL_Surface* sHP;
    if(SDL_GetTicks() - locTime > 100)
    {
        locTime = SDL_GetTicks();
        int hp = 0;
        CAliveMob* loc = castTo<CAliveMob>(*(mobMaster->thisMob));
       // if(loc) hp = loc->hp;
       std::string str//"HP: ";
        char num[10];
        _itoa_s(hp, num, 10);
        //str += num;
        _itoa_s(posx, num, 10);
        str += num;
        str += ", ";
        _itoa_s(posy, num, 10);
        str += num;
       // SDL_FreeSurface(sHP);
        SDL_Color color = {255, 0, 0, 0};
       // sHP = TTF_RenderText_Blended( map->aSpr.font, str.c_str(), color);
    };
    rect.x = 350;
    rect.y = 0;
    // TODO SDL_BlitSurface(sHP, NULL, mobMaster->screen, &rect);*/
}

void IMob::delThis()
{
    DeinitGUI();
    cautOverMind();
    IOnMapItem::delThis();
}

void IMob::processGUImsg(std::string& msg)
{
    int i;
    int& j = i;
    int& u = j;
    HashAmount h;
    std::list<HashAmount> hloc;
    if(msg == "SDLK_UP")
        checkMove(D_UP);
    else if(msg == "SDLK_DOWN")
        checkMove(D_DOWN);
    else if(msg == "SDLK_LEFT")
        checkMove(D_LEFT);
    else if(msg == "SDLK_RIGHT")
        checkMove(D_RIGHT);
    else if(msg == "SDLK_j")
        level = 1;
    else if(msg == "SDLK_1")
    {
        h.hash = hash("petrol");
        h.amount = 1;
        hloc.push_back(h);
        map->splashLiquid(hloc, posx, posy);
    }
    else if(msg == "SDLK_2")
    {
        h.hash = hash("honey");
        h.amount = 1;
        hloc.push_back(h);
        map->splashLiquid(hloc, posx, posy);
    }
    else if(msg == "SDLK_3")
    {
        h.hash = hash("water");
        h.amount = 3;
        hloc.push_back(h);
        map->splashLiquid(hloc, posx, posy);
    }
    else if(msg == "SDLK_f")
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