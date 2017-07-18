#include "LoginMob.h"

#include "representation/Sound.h"
#include "Human.h"
#include "core/ObjectFactory.h"
#include "core/Map.h"
#include "core/Game.h"
#include "core/objects/Lobby.h"
#include "net/MagicStrings.h"
#include "core/Helpers.h"
#include "representation/Representation.h"
#include "core/objects/Professions.h"

#include <QDebug>

const char* LOGIN_CLICK = "login_click";

using namespace kv;

LoginMob::LoginMob()
{
    name = "LobbyPlayer";

    interface_.Init();
}

void LoginMob::AfterWorldCreation()
{
    Mob::AfterWorldCreation();

    name = QString("LobbyPlayer%1").arg(GetId());
}

void LoginMob::MindExit()
{
    PlayMusic("");
}

void LoginMob::MindEnter()
{
    PlayMusic("lobby.ogg", 10);
}


void LoginMob::GenerateInterfaceForFrame()
{
    interface_.Draw();
}

const QString GENERIC_TEXT =
    "A space anomaly has moved a piece of your Space Station into an unknown part of space."
    " All laws and rules are gone, and you are just a poor <b>%1</b>."
    " Seems like atmos still works, so maybe you won't die from asphyxiation.<br>";

const int HUMAN_STATES_AMOUNT = 12;
const QString HUMAN_STATES[HUMAN_STATES_AMOUNT] =
    {"caucasian1_m_s", "caucasian2_m_s", "caucasian3_m_s",
     "latino_m_s", "mediterranean_m_s", "asian1_m_s",
     "asian2_m_s", "arab_m_s", "indian_m_s",
     "african1_m_s", "african2_m_s", "albino_m_s"};

void LoginMob::ProcessMessage(const Message& msg)
{
    QJsonObject obj = Network2::ParseJson(msg);

    if (    msg.type == MessageType::ORDINARY
         && Network2::IsKey(obj, LOGIN_CLICK))
    {
        if (GetGame().GetGlobals()->lobby->GetSecondUntilStart() > 0)
        {
            return;
        }
        qDebug() << "Begin human creation in LoginMob";
        if (!IsMinded())
        {
            return;
        }
        IdPtr<Human> human = Create<Human>(Human::GetTypeStatic());

        QString human_state = HUMAN_STATES[GenerateRandom() % HUMAN_STATES_AMOUNT];
        human->SetState(human_state);

        IdPtr<Lobby> lobby = GetGame().GetGlobals()->lobby;
        std::vector<IdPtr<CubeTile>> tiles;
        QString text;
        switch (GenerateRandom() % 5)
        {
        case 0:
            professions::ToSecurityOfficer(human);
            tiles = lobby->GetTilesFor("security");
            text = GENERIC_TEXT.arg("security officer");
            break;
        case 1:
            professions::ToDoctor(human);
            tiles = lobby->GetTilesFor("doctor");
            text = GENERIC_TEXT.arg("doctor");
            break;
        case 2:
            professions::ToAssistant(human);
            tiles = lobby->GetTilesFor("assistant");
            text = GENERIC_TEXT.arg("assistant");
            break;
        case 3:
            professions::ToClown(human);
            tiles = lobby->GetTilesFor("clown");
            text = GENERIC_TEXT.arg("clown");
            break;
        case 4:
            professions::ToBarman(human);
            tiles = lobby->GetTilesFor("barman");
            text = GENERIC_TEXT.arg("barman");
            break;
        default:
            qDebug() << "Unknown profession id!";
        }

        if (tiles.empty())
        {
            auto& map = GetGame().GetMap();
            int x = map.GetWidth() / 2;
            int y = map.GetHeight() / 2;
            int z = map.GetDepth() / 2;
            tiles.push_back(map.At(x, y, z));
        }
        int index = GenerateRandom() % tiles.size();
        tiles[index]->AddObject(human);

        MoveMindTo(human);

        PostHtmlFor(text, human);

        qDebug() << "End human creation in LoginMob";
        Delete();
    }
}

void LoginMob::Process()
{

}

void LoginMob::CalculateVisible(VisiblePoints* visible_list) const
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
    Representation::InterfaceUnit unit;
    unit.name = LOGIN_CLICK;
    unit.pixel_x = 0;
    unit.pixel_y = 0;
    unit.view = view_;
    GetRepresentation().AddToNewFrame(unit);

    QString text = "TODO: LoginInterface should be inherited from kv::Object";
    /*if (GetGame()-GetSecondUntilStart() < 0)
    {
        text = "Round is in process, click on the screen";
    }
    else
    {
        text = QString("Until start: %1").arg(GetLobby().GetSecondUntilStart());
    }*/
    GetRepresentation().AddToNewFrame(Representation::TextEntry{"test", text});
}

unsigned int LoginInterface::hash() const
{
    return ::Hash(view_);
}
