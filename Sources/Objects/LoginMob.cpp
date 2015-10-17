#include "LoginMob.h"

#include "Text.h"
#include "sound.h"
#include "Human.h"
#include "ItemFabric.h"
#include "MapClass.h"
#include "Creator.h"
#include "Lobby.h"
#include "MagicStrings.h"
#include "Chat.h"
#include "helpers.h"

#include <QDebug>

const char* LOGIN_CLICK = "login_click";

LoginMob::LoginMob(size_t id) : IMob(id)
{
    std::stringstream conv;
    conv << "LobbyPlayer" << id;
    name = conv.str();

    interface_.Init();
}

void LoginMob::DeinitGUI()
{
    GetTexts().Delete("LoginScreen");
    GetTexts().Delete("LoginScreenCount");
    GetSoundPlayer().StopMusic();
}


void LoginMob::InitGUI()
{
    GetTexts()["LoginScreen"].SetUpdater
    ([this](std::string* str)
    {
        if (GetLobby().GetSecondUntilStart() <= 0)
        {
            *str = "CLICK ME";
        }
        else
        {
            *str = "";
        }
    }).SetFreq(1000)
      .SetSize(26)
      .SetPlace(sizeW / 2 - 144 + 16, sizeH / 2 - 13);

    GetTexts()["LoginScreenCount"].SetUpdater
    ([this](std::string* str)
    {
        std::stringstream conv;
        if (GetLobby().GetSecondUntilStart() < 0)
        {
            *str = "000";
            return;
        }
        conv << GetLobby().GetSecondUntilStart();
        *str = conv.str();
        if (str->length() == 2)
        {
            *str = "0" + *str;
        }
        else if (str->length() == 1)
        {
            *str = "00" + *str;
        }
    }).SetFreq(250)
      .SetSize(26)
      .SetPlace(sizeW / 2 - 26 - 13, sizeH / 2 + 30 + 13);

    GetSoundPlayer().PlayMusic("lobby.ogg", 10.0f);
}


void LoginMob::processGUI()
{
    interface_.Draw();
}


void LoginMob::processGUImsg(const Message2& msg)
{
    QJsonObject obj = Network2::ParseJson(msg);

    if (    msg.type == MessageType::ORDINARY
         && Network2::IsKey(obj, LOGIN_CLICK))
    {
        if (GetLobby().GetSecondUntilStart() > 0)
        {
            return;
        }
        size_t net_id = GetItemFabric()->GetNetId(GetId());
        if (net_id)
        {
            auto human = GetItemFabric()->newItem<Human>(CaucasianHuman::T_ITEM_S());
            //ghost->name = name;
            GetItemFabric()->SetPlayerId(net_id, human.ret_id());

            auto tiles = GetLobby().GetTilesFor("default");

            tiles[0]->AddItem(human);
            if (GetId() == GetMob().ret_id())
            {
                ChangeMob(human);
            }
        }
    }
    if (msg.type == MessageType::MESSAGE)
    {
        std::string text = obj["text"].toString().toStdString();
        if (Chat::IsOOCMessage(text))
        {
            Chat::GetChat()->PostOOCText(name, text.substr(3));
        }
    }
}

void LoginMob::process()
{

}

void LoginMob::CalculateVisible(std::list<point>* visible_list)
{
    visible_list->clear();
}

std::ostream& operator<<(std::stringstream& file, LoginInterface& interf)
{
    WrapWriteMessage(file, interf.view_);
    return file;
}
std::istream& operator>>(std::stringstream& file, LoginInterface& interf)
{
    WrapReadMessage(file, interf.view_);
    return file;
}

void LoginInterface::Init()
{
    view_.SetSprite("icons/login_screen.jpg");
}

bool LoginInterface::Click(int x, int y)
{
    helpers::normalize_pixels(&x, &y);

    bool is_tr = !view_.IsTransp(x, y, 0);

    if (is_tr)
    {
        Network2::GetInstance().SendOrdinaryMessage(QString::fromStdString(LOGIN_CLICK));
    }

    return is_tr;
}

void LoginInterface::Draw()
{
    view_.Draw(0, 0, 0);
}

unsigned int LoginInterface::hash() const
{
    return ::hash(view_);
}
