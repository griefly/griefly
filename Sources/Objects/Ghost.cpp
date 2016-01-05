#include "Ghost.h"

#include "Map.h"
#include "Creator.h"

#include "MagicStrings.h"
#include "Chat.h"
#include "ObjectFactory.h"
#include "Lobby.h"
#include "Text.h"
#include "LoginMob.h"

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

    SetFreq(10);

    seconds_until_respawn_ = 15;
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
    int x_low_border = std::max(0, GetX() - sizeHsq - 1);
    int x_high_border = std::min(GetMap().GetMapW(), GetX() + sizeHsq);
    int y_low_border = std::max(0, GetY() - sizeWsq - 2);
    int y_high_border = std::min(GetMap().GetMapH(), GetY() + sizeWsq);
    for (int i = x_low_border; i < x_high_border; ++i)
        for (int j = y_low_border; j < y_high_border; ++j)
        {
            p.posx = i;
            p.posy = j;
            visible_list->push_back(p);
        }
}

void Ghost::processGUImsg(const Message2& msg)
{
    IMob::processGUImsg(msg);

    QJsonObject obj = Network2::ParseJson(msg);
    if (msg.type == MessageType::MESSAGE)
    {
        std::string text = obj["text"].toString().toStdString();
        if (Chat::IsOOCMessage(text))
        {
            GetChat().PostOOCText(name + " (ghost)", text.substr(3));
        }
    }
}

void Ghost::InitGUI()
{
    GetTexts()["RespawnCount"].SetUpdater
    ([this](std::string* str)
    {
        std::stringstream conv;
        conv << "Until respawn: " << seconds_until_respawn_;
        *str = conv.str();
    }).SetFreq(250)
      .SetSize(17)
      .SetPlace(sizeW / 2 - 140, sizeH / 2 - 200)
      .SetColor(250, 140, 140);
}

void Ghost::DeinitGUI()
{
    GetTexts().Delete("RespawnCount");
}

void Ghost::Process()
{
    --seconds_until_respawn_;
    if (seconds_until_respawn_ < 0)
    {
        size_t net_id = GetFactory().GetNetId(GetId());
        if (net_id)
        {
            auto login_mob = GetFactory().Create<IMob>(LoginMob::T_ITEM_S());

            GetFactory().SetPlayerId(net_id, login_mob.ret_id());
            if (GetId() == GetMob().ret_id())
            {
                ChangeMob(login_mob);
            }
            delThis();
            //qDebug() << "Ghost deleted: net_id: " << net_id;
        }
    }
}
