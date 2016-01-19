#include "Game.h"

#include <memory>

#include "Map.h"
#include "representation/Text.h"

#include "SyncRandom.h"
#include "ObjectFactory.h"
#include "net/MagicStrings.h"
#include "objects/Tile.h"
#include "Debug.h"
#include "Params.h"

#include "objects/Creator.h"
#include "objects/Mob.h"
#include "representation/Utils.h"
#include "representation/Chat.h"
#include "Names.h"
#include "objects/Movable.h"
#include "objects/Human.h"
#include "objects/LoginMob.h"
#include "objects/Lobby.h"
#include "objects/SpawnPoints.h"

#include "AutogenMetadata.h"


#include "net/Network2.h"
#include "net/NetworkMessagesTypes.h"
#include "representation/Representation.h"

#include "representation/Screen.h"

#include <QCoreApplication>
#include <QJsonObject>
#include <QByteArray>
#include <QUuid>

int ping_send;

Manager::Manager()
{
    auto_player_ = false;
    visible_points_ = new std::list<point>;
    last_fps_ = FPS_MAX;

    ping_id_ = "";
    ping_send_time_ = 0;

    current_ping_ = 0;

    ping_send_is_requested_ = true;


    this->moveToThread(&thread_);
    connect(&thread_, &QThread::started, this, &Manager::process);
}

void Manager::UpdateVisible() 
{
    visible_points_->clear();

    GetMob()->CalculateVisible(visible_points_);
}

void Manager::Process()
{
    int begin_of_process;

    int delay = 0;
    int lastTimeFps = SDL_GetTicks();
    fps_ = 0;
    process_in_ = false;
    Timer tick_timer, draw_timer, process_timer, atmos_move_timer, force_timer;
    tick_timer.Start();
    draw_timer.Start();
    process_timer.Start();
    atmos_move_timer.Start();
    force_timer.Start();
    unsigned int draw_time_per_tick = 0;

    unsigned int last_effect_process = 0;

    unsigned int ping_send_time = 0;

    while (true)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 40);
        ProcessInputMessages();

        const int ATMOS_OFTEN = 1;
        const int ATMOS_MOVE_OFTEN = 1;

        if(process_in_)
        {
            ++MAIN_TICK;

            process_timer.Start();
            begin_of_process = SDL_GetTicks();
            GetFactory().ForeachProcess();
            force_timer.Start();
            ForceManager::Get().Process();
            unsigned int fm = force_timer.Get();
            if (ATMOS_OFTEN == 1 || MAIN_TICK % ATMOS_OFTEN == 1)
            {
                GetMap().atmosphere.Process();
            }
            atmos_move_timer.Start();
            if (ATMOS_MOVE_OFTEN == 1 || MAIN_TICK % ATMOS_MOVE_OFTEN == 1)
            {
                GetMap().atmosphere.ProcessMove();
            }
            unsigned int amt = atmos_move_timer.Get();
            GetFactory().Sync();
            //SYSTEM_STREAM << "Processing take: " << (SDL_GetTicks() - begin_of_process) / 1000.0 << "s" << std::endl;
 
            //////////////////////////////
            //SYSTEM_STREAM << tick_timer.Get() / 1000.0 << std::endl;
            //SYSTEM_STREAM << draw_time_per_tick / 1000.0 << std::endl;

            if (MAIN_TICK % 10 == 0)
            {
               // SYSTEM_STREAM << "Draw take: " << (draw_time_per_tick * 1.0 / tick_timer.Get()) * 100.0 << "%" << std::endl;
              //  SYSTEM_STREAM << "Process take: " << (process_timer.Get() * 1.0 / tick_timer.Get()) * 100.0 << "%" << std::endl;
              //  SYSTEM_STREAM << "Atmos move take: " << (amt * 1.0 / tick_timer.Get()) * 100.0 << "%" << std::endl;
              //  SYSTEM_STREAM << "Force take: " << (fm * 1.0 / tick_timer.Get()) * 100.0 << "%" << std::endl;
            }
            tick_timer.Start();
            draw_time_per_tick = 0;
        
          //  Debug::UnsyncDebug().CheckSaves();
            GetMap().GenerateFrame();
        }

        if((SDL_GetTicks() - lastTimeFps) >= 1000)
        {
            UpdateVisible();

            lastTimeFps = SDL_GetTicks();
            last_fps_ = fps_;
            fps_ = 0;
        }

        if (   (SDL_GetTicks() - ping_send_time) > 1000
            && ping_send_is_requested_)
        {
            ping_id_ = QUuid::createUuid().toString();
            Network2::GetInstance().SendPing(ping_id_);
            ping_send_time_ = SDL_GetTicks();
            ping_send_is_requested_ = false;
        }

        ++fps_;
        process_in_ = false;
        // TODO: make it working in multithread

        /*if (Network2::GetInstance().IsGood() == false)
        {

        }

        Debug::UnsyncDebug().ProcessDebug();

        if (GetMainWidget().isHidden())
        {
            break;
        }*/
    }
}

const std::string ON_LOGIN_MESSAGE =
        "Welcome to Griefly! It is yet another space station remake, so if you are here then you probably already know how to play."
        " Just in case: arrows for movement, left mouse click for hand actions (hit, attack, take), chat for speaking."
        " Use prefix ooc in the chat if you would like to use the ooc channel (it is a global channel)."
        " The special button is '`' (tilde button) - it shows the current scoreboard.\n\n";

void Manager::InitWorld(int id, std::string map_name)
{
    InitSettersForTypes();

    std::cout << "Begin init world" << std::endl;

    std::cout << "Begin set manager" << std::endl;
    SetManager(this);

    SetFactory(new ObjectFactory);
    SetMapMaster(new MapMaster);

    LoadNames();

    std::cout << "Create tiles" << std::endl;
    int x = GetParamsHolder().GetParamBool("map_x") ? GetParamsHolder().GetParam<int>("map_x") : 40;
    int y = GetParamsHolder().GetParamBool("map_y") ? GetParamsHolder().GetParam<int>("map_y") : 40;
    int z = GetParamsHolder().GetParamBool("map_z") ? GetParamsHolder().GetParam<int>("map_z") : 1;
    GetMap().MakeTiles(x, y, z);

    std::cout << "Begin choose map" << std::endl;
    if (map_name == "no_map")
    {
        if (   GetParamsHolder().GetParamBool("mapgen_name")
            && utils::IsFileExist(GetParamsHolder().GetParam<std::string>("mapgen_name")))
        {
            srand(SDL_GetTicks());

            GetMap().LoadFromMapGen(GetParamsHolder().GetParam<std::string>("mapgen_name"));

            GetFactory().Create<Lobby>(Lobby::T_ITEM_S());

            for (auto it = GetFactory().GetIdTable().begin();
                      it != GetFactory().GetIdTable().end();
                      ++it)
            {
                if ((*it) && ((*it)->RT_ITEM() == SpawnPoint::REAL_TYPE_ITEM))
                {
                    GetLobby().AddSpawnPoint((*it)->GetId());
                }
            }

            auto newmob = GetFactory().Create<IMob>(LoginMob::T_ITEM_S());


            ChangeMob(newmob);
            GetFactory().SetPlayerId(id, newmob.ret_id());

            GetMap().FillAtmosphere();

        }
        else
        {
            // TODO: do smth
            return;
        }
    }
    else
    {
        qDebug() << "Begin load map";

        QByteArray map_data = Network2::GetInstance().GetMapData();

        std::stringstream ss;
        ss.write(map_data.data(), map_data.length());
        ss.seekg(0, std::ios::beg);

        GetFactory().LoadMap(ss, false, id);
    }

    GetChat().PostText(ON_LOGIN_MESSAGE);

    GetTexts()["FPS"].SetUpdater
    ([this](std::string* str)
    {
        std::stringstream ss; 
        ss << last_fps_ - 1;
        *str = ss.str();
    }).SetFreq(1000).SetSize(20);

    GetTexts()["Sync"].SetUpdater
    ([&](std::string* str)
    {
        std::stringstream ss;
        ss << "Hash: " << GetFactory().GetLastHash();
        *str = ss.str();
    }).SetSize(15).SetPlace(0, 30);//, 200, 50);

    GetTexts()["MorePreciseSync"].SetUpdater
    ([&](std::string* str)
    {
        std::stringstream ss;
        if (Debug::UnsyncDebug().IsReportGenerated())
        {
            ss << "!!REPORT!!";
        }
        *str = ss.str();
    }).SetSize(15).SetPlace(200, 30).SetColor(200, 0, 0);

   /* GetTexts()["SyncTick"].SetUpdater
    ([&](std::string* str)
    {
        std::stringstream ss;
        ss << NetClient::GetNetClient()->HashTick();
        ss >> *str;
    }).SetSize(15).SetPlace(120, 0).SetColor(150, 0, 0);*/

    GetTexts()["Tick"].SetUpdater
    ([&](std::string* str)
    {
        std::stringstream ss;
        ss << MAIN_TICK;
        *str = ss.str();
    }).SetSize(15).SetPlace(120, 0).SetColor(150, 0, 0);

    GetTexts()["PingTimer"].SetUpdater
    ([&](std::string* str)
    {
        std::stringstream ss;
        ss << "Ping: " << current_ping_ << "ms";
        *str = ss.str();
    }).SetSize(15).SetPlace(300, 0).SetColor(0, 140, 0);

    GetTexts()["LastTouch"].SetUpdater
    ([this](std::string* str)
    {
        *str = last_touch_;
    }).SetFreq(20).SetPlace(0, 485).SetSize(22);


    thread_.start();
}

void Manager::ProcessInputMessages()
{
    while (Network2::GetInstance().IsMessageAvailable())
    {
        Message2 msg = Network2::GetInstance().PopMessage();

        if (msg.type == MessageType::NEW_TICK)
        {
            process_in_ = true;
            break;
        }
        if (msg.type == MessageType::NEW_CLIENT)
        {
            QJsonObject obj = Network2::ParseJson(msg);

            QJsonValue new_id_v = obj["id"];
            int new_id = new_id_v.toVariant().toInt();

            size_t game_id = GetFactory().GetPlayerId(new_id);

            if (game_id != 0)
            {
                qDebug() << "game_id " << game_id << " already exists";
                continue;
            }

            auto newmob = GetFactory().Create<IMob>(LoginMob::T_ITEM_S());

            qDebug() << "New client " << newmob.ret_id();

            GetFactory().SetPlayerId(new_id, newmob.ret_id());
            continue;
        }
        if (msg.type == MessageType::MAP_UPLOAD)
        {
            QJsonObject obj = Network2::ParseJson(msg);
            QString map_url = obj["url_to_upload_map"].toString();

            qDebug() << "Map upload to " << map_url;

            std::stringstream ss;
            GetFactory().SaveMap(ss, false);
            std::string string_data = ss.str();

            QByteArray data(string_data.c_str(), string_data.size());

            Network2::GetInstance().SendMap(map_url, data);
            continue;
        }

        if (msg.type == MessageType::HASH)
        {
            QJsonObject obj = Network2::ParseJson(msg);

            QJsonValue hash_v = obj["hash"];
            int hash = hash_v.toVariant().toInt();

            QJsonValue tick_v = obj["tick"];
            int tick = tick_v.toVariant().toInt();

            Debug::UnsyncDebug().AddNetSyncPair(hash, tick);

            continue;
        }

        if (msg.type == MessageType::PING)
        {
            QJsonObject obj = Network2::ParseJson(msg);

            QString ping_id = obj["ping_id"].toString();

            if (ping_id != ping_id_)
            {
                continue;
            }

            current_ping_ = SDL_GetTicks() - ping_send_time_;
            ping_send_is_requested_ = true;
            continue;
        }

        if (   msg.type == MessageType::ORDINARY
            || msg.type == MessageType::MOUSE_CLICK
            || msg.type == MessageType::MESSAGE)
        {
            QJsonObject obj = Network2::ParseJson(msg);
            QJsonValue v = obj["id"];
            int net_id = v.toVariant().toInt();
            size_t game_id = GetFactory().GetPlayerId(net_id);
            if (game_id == 0)
            {
                qDebug() << "Game id is 0";
            }
            id_ptr_on<IMessageReceiver> game_object = game_id;

            if (game_object.valid())
            {
                game_object->processGUImsg(msg);
            }
            else
            {
                qDebug() << "Game object is not valid: " << net_id;
                abort();
            }
        }

        // TODO: other stuff
    }
}

void Manager::process()
{
    Process();
}

bool Manager::IsMobVisible(int posx, int posy)
{
    // TODO: matrix for fast check
    if (visible_points_ == nullptr)
        return false;
    for (auto it = visible_points_->begin(); it != visible_points_->end(); ++it)
        if(it->posx == posx && it->posy == posy)
            return true;
    return false;
}

QWidget* main_widget = nullptr;
QWidget& GetMainWidget()
{
    return *main_widget;
}

void SetMainWidget(QWidget* widget)
{
    main_widget = widget;
}

Manager* manager_ = nullptr;
Manager& GetManager()
{
    return *manager_;
}

void SetManager(Manager* manager)
{
    manager_ = manager;
}

bool IsManagerValid()
{
    return manager_ != nullptr;
}
