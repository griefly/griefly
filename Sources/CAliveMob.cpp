#include <algorithm>

#include "NetClientImpl.h"

#include "CAliveMob.h"
#include "MapClass.h"
#include "Text.h"


#include "CSmallItem.h"
#include "LiquidHolder.h"
#include "ASpritesClass.h"
#include "sprite.h"
#include "ItemFabric.h"
#include "FireEffect.h"
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
    tick_sm = 0;
};

void CAliveMob::process()
{
    IMob::process();
    if(!onMobControl) aaMind();
    live();
};

void CAliveMob::ignite(int force)
{
    SYSTEM_STREAM << "Ignite\n";
    Fire* eff = EffectFabricOf<Fire>::getEffectOf();
    SYSTEM_STREAM << GetId() << "ignite\n";
    SYSTEM_STREAM << eff << "\n";
    eff->master = GetId();
    eff->Start();
    burn_power += force;
}

void CAliveMob::aaMind() {};

void CAliveMob::live()
{ 
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
    GetTexts()["Sync"].SetUpdater
    ([&](std::string* str)
    {
        std::stringstream ss;
        ss << ((GetItemFabric()->get_hash_last() == NetClient::GetNetClient()->Hash()) ? "SYNC:" : "UNSYNC:") 
           << GetItemFabric()->get_hash_last();
        ss >> *str;
    }).SetSize(15).SetPlace(0, 30, 200, 50);
}

void CAliveMob::DeinitGUI()
{
    GetTexts().Delete("Sync");
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
        //ssloc << inside->amountOf(hash("liquidblood")) << "bl " << dmg << "d " << injuries << "i " << burn << "b " << interior << "in\n " << bloodless << "b " << oxyless << "o";
        SDL_Color color = {255, 0, 0, 0};
        //if(sDMG != nullptr)
        //    SDL_FreeSurface(sDMG);
        //sDMG = TTF_RenderText_Blended( map->aSpr.font, ssloc.str().c_str(), color);

        if (GetItemFabric()->get_hash_last() == NetClient::GetNetClient()->Hash())
            GetTexts()["Sync"].SetColor(0, 255, 100);
        else
            GetTexts()["Sync"].SetColor(255, 160, 0);
    }
}