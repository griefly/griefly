#include "CAliveMob.h"

#include <algorithm>

#include "NetClientImpl.h"
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

void CAliveMob::aaMind() {};

void CAliveMob::live()
{ 
    if (burn_power > 0)
        burn_power -= 1;
};

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
    static int locTime = 0;
    if(SDL_GetTicks() - locTime > 100)
    {
        SDL_Color color = {255, 0, 0, 0};
        if (GetItemFabric()->get_hash_last() == NetClient::GetNetClient()->Hash())
            GetTexts()["Sync"].SetColor(0, 255, 100);
        else
            GetTexts()["Sync"].SetColor(255, 160, 0);
    }
}