#include <algorithm>

#include "CAliveMob.h"
#include "MapClass.h"

#include "LiquidHolder.h"
#include "ASpritesClass.h"
#include "sprite.h"

#include "FireEffect.h"

#include "NetClass.h"
#include "sync_random.h"

CAliveMob::CAliveMob()
{
    dmg = 0;
    injuries = 0;
    burn = 0;
    interior = 0;//blood&digestive system dmg
    bloodless = 0;
    oxyless = 0;

    max_dmg = 100;
    inside = fabric->newItem<LiquidHolder>(id, hash("liquidholder"));
    tick_sm = 0;
};

void CAliveMob::process()
{
    if(!onMobControl) aaMind();
    live();
};

void CAliveMob::ignite(int force)
{
    SYSTEM_STREAM << "Ignite\n";
    Fire* eff = getEffectOf<Fire>();
    SYSTEM_STREAM << id << "ignite\n";
    SYSTEM_STREAM << eff << "\n";
    eff->master = id;
    eff->Start();
    burn_power += force;
}

void CAliveMob::aaMind() {};

void CAliveMob::live()
{ 
    unsigned int now_blood = inside->amountOfAll();
    if(injuries > 1 && now_blood > 0 && get_rand() % std::max(1, max_dmg * 7 - injuries * 7) == 0)
        (*map->squares[posx][posy].begin())->insertLiquid(inside->removeLiquid(injuries));
    if (burn_power > 0)
        burn_power -= 1;
};

/*bool CAliveMob::checkMove(Dir direct)
{
    if(!checkMoveTime()) return false;
    int locx = posx;
    int locy = posy;
    if(direct == D_UP) locy -= 1;
    if(direct == D_DOWN) locy += 1;
    if(direct == D_LEFT) locx -= 1;
    if(direct == D_RIGHT) locx += 1;
    auto itr = map->squares[locx][locy].begin();
    /*while(itr != map->mobs[locx][locy].end())
    {
        CAliveMob* loc;
        loc = isType<CAliveMob>(**itr);
        if(loc)
        {
            loc->attack_by(id);
            return false;
        }
        itr++;
    }
    dMove = direct;
    if(!checkPassable()) return false;
    return mainMove(); 
};*/

void CAliveMob::attack_by(id_ptr_on<SmallItem> atk, int force)
{
    dmg += atk->force;
    injuries += atk->penet_force;
};

void CAliveMob::InitGUI()
{
    mobMaster->texts["Sync"].SetUpdater
    ([&](std::string* str)
    {
        std::stringstream ss;
        ss << ((fabric->get_hash_last() == NetMaster->hash) ? "SYNC:" : "UNSYNC:") << fabric->get_hash_last();
        ss >> *str;
    }).SetSize(15).SetPlace(0, 30, 200, 50);
}

void CAliveMob::DeinitGUI()
{
    mobMaster->texts.Delete("Sync");
}

void CAliveMob::processGUI()
{
    IMob::processGUI();
    static SDL_Surface* sDMG = nullptr;
    static SDL_Surface* sync = nullptr;
    static int locTime = 0;
    std::stringstream ssloc, ssloc2;
    if(SDL_GetTicks() - locTime > 100)
    {
        ssloc << inside->amountOf(hash("liquidblood")) << "bl " << dmg << "d " << injuries << "i " << burn << "b " << interior << "in\n " << bloodless << "b " << oxyless << "o";
        SDL_Color color = {255, 0, 0, 0};
        //if(sDMG != nullptr)
        //    SDL_FreeSurface(sDMG);
        //sDMG = TTF_RenderText_Blended( map->aSpr.font, ssloc.str().c_str(), color);

        if (fabric->get_hash_last() == NetMaster->hash)
            mobMaster->texts["Sync"].SetColor(0, 255, 100);
        else
            mobMaster->texts["Sync"].SetColor(255, 160, 0);
        ssloc2 << ((fabric->get_hash_last() == NetMaster->hash) ? "SYNC" : "UNSYNC") << fabric->get_hash_last();
        if (MAIN_TICK % HASH_OFTEN == 55)
        {
          //  if(sync != nullptr)
          //      SDL_FreeSurface(sync);
          //  sync = TTF_RenderText_Blended( map->aSpr.font, ssloc2.str().c_str(), color2);
        }
    }
    SDL_Rect rect;
    rect.x = 150;
    rect.y = 25;
    // TODO
    // if(sDMG != nullptr)
    //    SDL_BlitSurface(sDMG, NULL, mobMaster->screen, &rect);
    rect.x = 15;
    rect.y = 35;
    // if(sDMG != nullptr)
    //    SDL_BlitSurface(sync, NULL, mobMaster->screen, &rect);
}