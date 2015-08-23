#include "LoginMob.h"

#include "Text.h"
#include "sound.h"

LoginMob::LoginMob(size_t id) : IMob(id)
{
    //SetFreq(0);
    SetSprite("icons/288x288.dmi");
    SetState("singularity_s9");
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

    GetSoundPlayer().PlayMusic("lobby.ogg", 70.0f);
}


void LoginMob::processGUI()
{
    DrawMain(0, sizeW / 2 - 144, sizeH / 2 - 144);
}


void LoginMob::processGUImsg(const Message& msg)
{

}

void LoginMob::process()
{

}

void LoginMob::CalculateVisible(std::list<point>* visible_list)
{
    visible_list->clear();
}
