#include "Ghost.h"

#include "../Map.h"
#include "Creator.h"

#include "net/MagicStrings.h"
#include "representation/Chat.h"
#include "../ObjectFactory.h"
#include "Lobby.h"
#include "representation/Text.h"
#include "LoginMob.h"
#include "../Game.h"

#include "Human.h"

Ghost::Ghost(size_t id) : IMob(id)
{
    tickSpeed = 1;
    pixSpeed = 2;

    v_level = 11;

    SetSprite("icons/mob.dmi");
    SetState("ghost_grey");

    passable_level = Passable::EMPTY;

    name = "Ghost";

    seconds_until_respawn_ = 15;
}

void Ghost::AfterWorldCreation()
{
    IMob::AfterWorldCreation();
    SetFreq(10);
}

bool Ghost::IsMobGhost()
{
    static size_t mob_id = 0;
    static bool draw = true;
    if (!GetMob())
        return false;
    if (mob_id != GetMob().ret_id())
    {
        if (id_ptr_on<Ghost> g = GetMob())
            draw = true;
        else
            draw = false;
        mob_id = GetMob().ret_id();
    }
    return draw;
}

void Ghost::Represent()
{
    if (IsMobGhost())
    {
        IMob::Represent();
    }
}

void Ghost::CalculateVisible(std::list<point>* visible_list)
{
    visible_list->clear();
    point p;
    p.posz = GetZ();
    int x_low_border = std::max(0, GetX() - SIZE_H_SQ - 1);
    int x_high_border = std::min(game_->GetMap().GetMapW(), GetX() + SIZE_H_SQ);
    int y_low_border = std::max(0, GetY() - SIZE_W_SQ - 2);
    int y_high_border = std::min(game_->GetMap().GetMapH(), GetY() + SIZE_W_SQ);
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

void Ghost::processGUImsg(const Message2& msg)
{
    IMob::processGUImsg(msg);

    /*QJsonObject obj = Network2::ParseJson(msg);
    if (msg.type == MessageType::MESSAGE)
    {
        std::string text = obj["text"].toString().toStdString();
        if (Chat::IsOOCMessage(text))
        {
            GetChat().PostOOCText(name + " (ghost)", text.substr(3));
        }
    }*/
}

void Ghost::InitGUI()
{
    game_->GetTexts()["RespawnCount"].SetUpdater
    ([this](std::string* str)
    {
        std::stringstream conv;
        conv << "Until respawn: " << seconds_until_respawn_;
        *str = conv.str();
    }).SetFreq(250);
}

void Ghost::DeinitGUI()
{
    game_->GetTexts().Delete("RespawnCount");
}

void Ghost::Process()
{
    --seconds_until_respawn_;
    if (seconds_until_respawn_ < 0)
    {
        size_t net_id = game_->GetFactory().GetNetId(GetId());
        if (net_id)
        {
            auto login_mob = game_->GetFactory().Create<IMob>(LoginMob::T_ITEM_S());

            game_->GetFactory().SetPlayerId(net_id, login_mob.ret_id());
            if (GetId() == GetMob().ret_id())
            {
                ChangeMob(login_mob);
            }
            Delete();
            //qDebug() << "Ghost deleted: net_id: " << net_id;
        }
    }
}
