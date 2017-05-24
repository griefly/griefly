#include "Ghost.h"

#include "../Map.h"

#include "net/MagicStrings.h"
#include "representation/Chat.h"
#include "../ObjectFactory.h"
#include "Lobby.h"
#include "representation/Text.h"
#include "LoginMob.h"
#include "../Game.h"

#include "Human.h"

using namespace kv;

Ghost::Ghost()
{
    tick_speed_ = 1;

    v_level = 11;

    SetSprite("icons/mob.dmi");
    SetState("ghost_grey");

    passable_level = passable::EMPTY;

    name = "Ghost";

    seconds_until_respawn_ = 15;
}

void Ghost::AfterWorldCreation()
{
    Mob::AfterWorldCreation();
    SetFreq(10);
}

bool Ghost::IsMobGhost()
{
    static quint32 mob_id = 0;
    static bool draw = true;
    if (!GetGame().GetMob())
    {
        return false;
    }
    if (mob_id != GetGame().GetMob().Id())
    {
        if (IdPtr<Ghost> g = GetGame().GetMob())
        {
            draw = true;
        }
        else
        {
            draw = false;
        }
        mob_id = GetGame().GetMob().Id();
    }
    return draw;
}

void Ghost::Represent()
{
    if (IsMobGhost())
    {
        Mob::Represent();
    }
}

void Ghost::CalculateVisible(std::list<PosPoint>* visible_list)
{
    visible_list->clear();
    PosPoint p;
    p.posz = GetZ();
    int x_low_border = std::max(0, GetX() - SIZE_H_SQ);
    int x_high_border = std::min(GetGame().GetMap().GetWidth(), GetX() + SIZE_H_SQ);
    int y_low_border = std::max(0, GetY() - SIZE_W_SQ);
    int y_high_border = std::min(GetGame().GetMap().GetHeight(), GetY() + SIZE_W_SQ);
    for (int i = x_low_border; i < x_high_border; ++i)
    {
        for (int j = y_low_border; j < y_high_border; ++j)
        {
            p.posx = i;
            p.posy = j;
            visible_list->push_back(p);
        }
    }
}

void Ghost::ProcessMessage(const Message2& msg)
{
    Mob::ProcessMessage(msg);
}

void Ghost::InitGui()
{
    GetGame().GetTexts()["RespawnCount"].SetUpdater
    ([this](QString* str)
    {
        *str = QString("Until respawn: %1").arg(seconds_until_respawn_);
    }).SetFreq(250);
}

void Ghost::DeinitGui()
{
    GetGame().GetTexts().Delete("RespawnCount");
}

void Ghost::Process()
{
    --seconds_until_respawn_;
    if (seconds_until_respawn_ < 0)
    {
        if (IsMinded())
        {
            auto login_mob = Create<Mob>(LoginMob::GetTypeStatic(), 0);
            MoveMindTo(login_mob);
            Delete();
        }
    }
}
