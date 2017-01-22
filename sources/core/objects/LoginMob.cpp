#include "LoginMob.h"

#include "representation/Text.h"
#include "representation/Sound.h"
#include "Human.h"
#include "../ObjectFactory.h"
#include "../Map.h"
#include "../Game.h"
#include "Lobby.h"
#include "net/MagicStrings.h"
#include "representation/Chat.h"
#include "../Helpers.h"
#include "representation/Representation.h"
#include "Professions.h"

#include <QDebug>

const char* LOGIN_CLICK = "login_click";

LoginMob::LoginMob(quint32 id) : IMob(id)
{
    name = QString("LobbyPlayer%1").arg(id);

    interface_.Init();
}

void LoginMob::DeinitGUI()
{
    GetGame().GetTexts().Delete("LoginScreenCount");
    PlayMusic("");
}


void LoginMob::InitGUI()
{
    GetGame().GetTexts()["LoginScreenCount"].SetUpdater
    ([this](QString* str)
    {
        if (GetLobby().GetSecondUntilStart() < 0)
        {
            *str = "Round is in process, click on the screen";
            return;
        }
        else
        {
            *str = QString::number(GetLobby().GetSecondUntilStart());
        }
        *str = "Until start: " + *str;
    });

    PlayMusic("lobby.ogg", 10.0f);
}


void LoginMob::GenerateInterfaceForFrame()
{
    interface_.Draw();
}

const QString SECURITY_TEXT =
        "A space anomaly has moved a piece of your Space Station into an unknown part of space."
        " All laws and rules are gone, so you can finally fulfil your dearest wish: to kill all janitors."
        " Good luck on the battlefield, and remember - the atmos is missing, so be quick to act.\n";
const QString JANITOR_TEXT =
        "A space anomaly has moved a piece of your Space Station into an unknown part of space."
        " All laws and rules are gone, so you can finally fulfil your dearest wish: to kill all security."
        " Good luck on the battlefield, and remember - the atmos is missing, so be quick to act.\n";

void LoginMob::ProcessMessage(const Message2& msg)
{
    QJsonObject obj = Network2::ParseJson(msg);

    if (    msg.type == MessageType::ORDINARY
         && Network2::IsKey(obj, LOGIN_CLICK))
    {
        if (GetLobby().GetSecondUntilStart() > 0)
        {
            return;
        }
        qDebug() << "Begin human creation in LoginMob";
        quint32 net_id = GetGame().GetFactory().GetNetId(GetId());
        if (net_id)
        {
            IdPtr<Human> human = Create<Human>(Human::T_ITEM_S());;
            std::vector<IdPtr<CubeTile>> tiles;
            QString text;
            switch (GetRand() % 4)
            {
            case 0:
                professions::ToSecurityOfficer(human);
                tiles = GetLobby().GetTilesFor("security");
                text = SECURITY_TEXT;
                break;
            case 1:
                professions::ToDoctor(human);
                tiles = GetLobby().GetTilesFor("doctor");
                // text = DOCTOR_TEXT;
                break;
            case 2:
                professions::ToAssistant(human);
                tiles = GetLobby().GetTilesFor("assistant");
                // text = ASSISTANT_TEXT;
                break;
            case 3:
                professions::ToClown(human);
                tiles = GetLobby().GetTilesFor("clown");
                // text = CLOWN_TEXT;
                break;
            case 4:
                professions::ToBarman(human);
                tiles = GetLobby().GetTilesFor("barman");
                // text = CLOWN_TEXT;
            default:
                qDebug() << "Unknown profession id!";
            }

            GetGame().GetFactory().SetPlayerId(net_id, human.Id());

            if (tiles.empty())
            {
                auto& map = GetGame().GetMap();
                int x = map.GetWidth() / 2;
                int y = map.GetHeight() / 2;
                int z = map.GetDepth() / 2;
                tiles.push_back(map.GetSquares()[x][y][z]);
            }
            int index = GetRand() % tiles.size();
            tiles[index]->AddItem(human);
            if (GetId() == GetGame().GetMob().Id())
            {
                GetGame().ChangeMob(human);
            }

            GetGame().GetChat().PostTextFor(text, human);
        }
        qDebug() << "End human creation in LoginMob";
    }
}

void LoginMob::Process()
{

}

void LoginMob::CalculateVisible(std::list<PosPoint>* visible_list)
{
    visible_list->clear();
}

FastSerializer& operator<<(FastSerializer& file, LoginInterface& interf)
{
    WrapWriteMessage(file, interf.view_);
    return file;
}
FastDeserializer& operator>>(FastDeserializer& file, LoginInterface& interf)
{
    WrapReadMessage(file, interf.view_);
    return file;
}

void LoginInterface::Init()
{
    view_.SetSprite("icons/login_screen.jpg");
}

void LoginInterface::Draw()
{
    //view_.Draw(0, 0, 0);
    Representation::InterfaceUnit unit;
    unit.name = LOGIN_CLICK;
    unit.pixel_x = 0;
    unit.pixel_y = 0;
    unit.view = view_;
    GetRepresentation().AddToNewFrame(unit);
    //qDebug() << "Login interface";
}

unsigned int LoginInterface::hash() const
{
    return ::hash(view_);
}
