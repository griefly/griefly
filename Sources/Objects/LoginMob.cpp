#include "LoginMob.h"

#include "Text.h"
#include "sound.h"
#include "Human.h"
#include "ItemFabric.h"
#include "MapClass.h"
#include "Creator.h"

const char* LOGIN_CLICK = "login_click";

LoginMob::LoginMob(size_t id) : IMob(id)
{
    //SetFreq(0);
    interface_.Init();
}

void LoginMob::DeinitGUI()
{
    GetTexts().Delete("LoginScreen");
    GetSoundPlayer().StopMusic();
}


void LoginMob::InitGUI()
{
    GetTexts()["LoginScreen"].SetUpdater
    ([this](std::string* str)
    {
        *str = "CLICK ME";
    }).SetFreq(1000)
      .SetSize(26)
      .SetPlace(sizeW / 2 - 144 + 16, sizeH / 2 - 13);

    GetSoundPlayer().PlayMusic("lobby.ogg", 10.0f);
}


void LoginMob::processGUI()
{
    interface_.Draw();
}


void LoginMob::processGUImsg(const Message& msg)
{
    if (msg.text == LOGIN_CLICK)
    {
        size_t net_id = GetItemFabric()->GetNetId(GetId());
        if (net_id)
        {
            auto human = GetItemFabric()->newItem<Human>(Human::T_ITEM_S());
            //ghost->name = name;
            GetItemFabric()->SetPlayerId(net_id, human.ret_id());
            GetMapMaster()->
             squares[GetMapMaster()->GetMapW() / 2]
                    [GetMapMaster()->GetMapH() / 2]
                    [GetMapMaster()->GetMapD() / 2]->AddItem(human);
            if (GetId() == GetMob().ret_id())
            {
                ChangeMob(human);
            }
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
    view_.SetSprite("icons/288x288.dmi");
    view_.SetState("singularity_s9");
}

bool LoginInterface::Click(int x, int y)
{
    bool is_tr = !view_.IsTransp(x - (sizeW / 2 - 144), y - (sizeH / 2 - 144), 0);

    if (is_tr)
    {
        Message msg;
        msg.text = LOGIN_CLICK;
        NetClient::GetNetClient()->Send(msg);
    }

    return is_tr;
}

void LoginInterface::Draw()
{
    view_.Draw(0, sizeW / 2 - 144, sizeH / 2 - 144);
}

unsigned int LoginInterface::hash() const
{
    return ::hash(view_);
}
