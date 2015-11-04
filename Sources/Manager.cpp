#include "Manager.h"

#include <memory>

#include "Map.h"
#include "Text.h"

#include "mob_position.h"

#include "EffectSystem.h"
#include "MoveEffect.h"
#include "sync_random.h"
#include "ItemFabric.h"
#include "MagicStrings.h"
#include "Tile.h"
#include "Debug.h"
#include "Params.h"

#include "sound.h"
#include "Creator.h"
#include "SdlInit.h"
#include "Mob.h"
#include "utils.h"
#include "ImageLoader.h"
#include "SoundLoader.h"
#include "Chat.h"
#include "Names.h"
#include "Movable.h"
#include "Human.h"
#include "LoginMob.h"
#include "Lobby.h"
#include "SpawnPoints.h"

#include "AutogenMetadata.h"

#include "qtopengl.h"

#include "Network2.h"
#include "NetworkMessagesTypes.h"

#include <QCoreApplication>
#include <QJsonObject>
#include <QByteArray>
#include <QUuid>

int ping_send;

void Manager::CheckMove(Dir direct)
{
    //TODO
    for (int z = 0; z < GetMapMaster()->GetMapD(); ++z)
        for(int x = std::max(0, GetMob()->GetX() - sizeHsq);
            x <= std::min(GetMob()->GetX() + sizeHsq, GetMapMaster()->GetMapH() - 1); x++)
        {
            for(int y = std::max(0, GetMob()->GetY() - sizeWsq);
                y <= std::min(GetMob()->GetY() + sizeWsq, GetMapMaster()->GetMapW() - 1); y++)
            {
                GetMapMaster()->squares[x][y][z]->ForEach([&](id_ptr_on<IOnMapBase> item)
                {
                    Move* eff = EffectFabricOf<Move>::getEffectOf();
                    eff->Init(TITLE_SIZE, direct, GetMob()->pixSpeed, item);
                    eff->Start();
                });
                auto trf = GetMapMaster()->squares[x][y][z]->GetTurf();
                if (trf.valid())
                {
                    Move* eff = EffectFabricOf<Move>::getEffectOf();
                    eff->Init(TITLE_SIZE, direct, GetMob()->pixSpeed, trf);
                    eff->Start();
                }
            }
        }
}

Manager::Manager()
{
    auto_player_ = false;
    visible_points_ = new std::list<point>;
    last_fps_ = FPS_MAX;

    ping_id_ = "";
    ping_send_time_ = 0;

    current_ping_ = 0;

    ping_send_is_requested_ = true;
};

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

        HandleInput();

        ProcessInputMessages();

        const int ATMOS_OFTEN = 1;
        const int ATMOS_MOVE_OFTEN = 1;

        if(process_in_)
        {
            ++MAIN_TICK;

            process_timer.Start();
            begin_of_process = SDL_GetTicks();
            GetItemFabric().ForeachProcess();
            force_timer.Start();
            ForceManager::Get().Process();
            unsigned int fm = force_timer.Get();
            if (ATMOS_OFTEN == 1 || MAIN_TICK % ATMOS_OFTEN == 1)
                GetMapMaster()->atmosphere.Process();
            atmos_move_timer.Start();
            if (ATMOS_MOVE_OFTEN == 1 || MAIN_TICK % ATMOS_MOVE_OFTEN == 1)
                GetMapMaster()->atmosphere.ProcessMove();
            unsigned int amt = atmos_move_timer.Get();
            GetItemFabric().Sync();
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
        }
         
        if (!NODRAW)
        {   
            draw_timer.Start();
            MakeCurrentGLContext();
            GetScreen()->Clear();
            GetMapMaster()->Draw();
            if ((SDL_GetTicks() - last_effect_process) > (1000 / 60))
            {
                last_effect_process = SDL_GetTicks();
                FabricProcesser::Get()->process();
            }
            //ClearGUIZone();

            GetChat().Process();

            GetMob()->processGUI();
            
            GetTexts().Process();
            
            //glFinish();
            GetScreen()->Swap();
            draw_time_per_tick += draw_timer.Get();
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
        if (Network2::GetInstance().IsGood() == false)
        {

        }

        Debug::UnsyncDebug().ProcessDebug();

        if (GetMainWidget().isHidden())
        {
            break;
        }
    }
}

void Manager::HandleInput()
{
    QCoreApplication::processEvents(QEventLoop::AllEvents, 40);

    if (!auto_player_)
        return;

    int w = GetGLWidget()->width();
    int h = GetGLWidget()->height();

    if (rand() % 5 == 1)
    {
        ProcessClick(rand() % w, rand() % h);
    }
    if (rand() % 10 == 1)
    {
        HandleKeyboardDown(nullptr);
    }
}

void Manager::ProcessClick(int mouse_x, int mouse_y)
{
    if (GetMob()->GetInterface() && GetMob()->GetInterface()->Click(mouse_x, mouse_y))
    {
        last_touch_ = "Interface";
        return;
    }

    // TODO: Remake, huge exploit
    if (id_ptr_on<Human> h = GetMob())
    {
        if (h->GetLying() == true)
            return;
    }

    id_ptr_on<IOnMapObject> item;
    item = GetMapMaster()->Click(mouse_x, mouse_y);
    if (item)
    {
        Message2 msg;
        msg.type = MessageType::MOUSE_CLICK;
        msg.json = "{\"obj\":" + QString::number(item.ret_id()) + "}";

        Network2::GetInstance().SendMsg(msg);

        last_touch_ = item->name;
    }
}

void Manager::HandleKeyboardDown(QKeyEvent* event)
{
    std::string text;

    int val = rand();

    if ((!event && (val % 100 == 1)) || (event && event->key() == Qt::Key_8) && (event->modifiers() == Qt::KeypadModifier))
    {
        text = Input::MOVE_UP;
    }
    else if ((!event && (val % 100 == 2)) || (event && event->key() == Qt::Key_2) && (event->modifiers() == Qt::KeypadModifier))
    {
        text = Input::MOVE_DOWN;
    }
    else if ((!event && (val % 100 == 3)) || (event && event->key() == Qt::Key_6) && (event->modifiers() == Qt::KeypadModifier))
    {
        text = Input::MOVE_RIGHT;
    }
    else if ((!event && (val % 100 == 4)) || (event && event->key() == Qt::Key_4) && (event->modifiers() == Qt::KeypadModifier))
    {
        text = Input::MOVE_LEFT;
    }
    else if ((!event && (val % 100 == 5)) || (event && event->key() == Qt::Key_Up))
    {
        text = Input::MOVE_UP;
    }
    else if ((!event && (val % 100 == 6)) || (event && event->key() == Qt::Key_Down))
    {
        text = Input::MOVE_DOWN;
    }
    else if ((!event && (val % 100 == 7)) || (event && event->key() == Qt::Key_Right))
    {
        text = Input::MOVE_RIGHT;
    }
    else if ((!event&& (val % 100 == 8)) || (event && event->key() == Qt::Key_Left))
    {
        text = Input::MOVE_LEFT;
    }
    else if ((!event && (val % 100 == 9)) || (event && event->key() == Qt::Key_Q))
    {
        text = Input::KEY_Q;
    }
    else if ((!event && (val % 100 == 10)) || (event && event->key() == Qt::Key_W))
    {
        text = Input::KEY_W;
    }
    else if ((!event && (val % 100 == 11)) || (event && event->key() == Qt::Key_E))
    {
        text = Input::KEY_E;
    }
    else if ((!event && (val % 100 == 12)) || (event && event->key() == Qt::Key_R))
    {
        text = Input::KEY_R;
    }
    else if (/* NO! */ (event && event->key() == Qt::Key_F3))
    {
        Debug::UnsyncDebug().GenerateAndSaveReport();
        return;
    }
    else if (event && event->key() == Qt::Key_QuoteLeft)
    {
        GetTexts()["SecScore"].SetUpdater
        ([&](std::string* str)
        {
            std::stringstream ss;
            ss << "Security: " << GetLobby().security_score_;
            *str = ss.str();
        }).SetSize(15).SetPlace(10, 50).SetColor(140, 140, 240);
        GetTexts()["JanScore"].SetUpdater
        ([&](std::string* str)
        {
            std::stringstream ss;
            ss << "Janitors: " << GetLobby().janitors_score_;
            *str = ss.str();
        }).SetSize(15).SetPlace(10, 70).SetColor(140, 140, 240);
        return;
    }
    else
    {
        return;
    }

    Network2::GetInstance().SendOrdinaryMessage(QString::fromStdString(text));
}

void Manager::HandleKeyboardUp(QKeyEvent *event)
{
    if (event && event->key() == Qt::Key_QuoteLeft)
    {
        GetTexts().Delete("SecScore");
        GetTexts().Delete("JanScore");
        return;
    }
}

const std::string ON_LOGIN_MESSAGE =
        "Welcome to Griefly! It is yet another space station remake, so if you are here then you probably already know how to play."
        " Just in case: arrows for movement, left mouse click for hand actions (hit, attack, take), chat for speaking."
        " Use prefix ooc in the chat if you would like to use the ooc channel (it is a global channel)."
        " The special button is '`' (tilde button) - it shows the current scoreboard.\n\n";

void Manager::InitWorld(int id, std::string map_name)
{
    if (!GetParamsHolder().GetParamBool("-debug_to_chat"))
    {
        SetLogToFile();
    }


    InitSettersForTypes();

    std::cout << "Begin init world" << std::endl;
    if (!InitSDL())
    {
        SYSTEM_STREAM << "Fail SDL load" << std::endl;
    }
    //SDL_WM_SetCaption(Debug::GetUniqueName().c_str(), Debug::GetUniqueName().c_str());

    std::cout << "Begin set manager" << std::endl;
    SetManager(this);

    SetItemFabric(new ItemFabric);
    SetMapMaster(new MapMaster);
    std::cout << "Screen set" << std::endl;

    int old_size_w = GetGLWidget()->width();
    int old_size_h = GetGLWidget()->height();
    if (!NODRAW)
        SetScreen(new Screen(sizeW, sizeH));
    GetGLWidget()->resize(old_size_w, old_size_h);
    std::cout << "Screen has been set" << std::endl;
    SetTexts(new TextPainter);
    SetSpriter(new SpriteHolder);

    std::cout << "Begin load resources" << std::endl;
    LoadImages();
    LoadSounds();
    LoadNames();

    std::cout << "Create tiles" << std::endl;
    int x = GetParamsHolder().GetParamBool("map_x") ? GetParamsHolder().GetParam<int>("map_x") : 40;
    int y = GetParamsHolder().GetParamBool("map_y") ? GetParamsHolder().GetParam<int>("map_y") : 40;
    int z = GetParamsHolder().GetParamBool("map_z") ? GetParamsHolder().GetParam<int>("map_z") : 1;
    GetMapMaster()->MakeTiles(x, y, z);

    std::cout << "Begin choose map" << std::endl;
    if (map_name == "no_map")
    {
        if (   GetParamsHolder().GetParamBool("mapgen_name")
            && utils::IsFileExist(GetParamsHolder().GetParam<std::string>("mapgen_name")))
        {
            srand(SDL_GetTicks());

            GetMapMaster()->LoadFromMapGen(GetParamsHolder().GetParam<std::string>("mapgen_name"));

            GetItemFabric().newItem<Lobby>(Lobby::T_ITEM_S());

            for (auto it = GetItemFabric().GetIdTable().begin();
                      it != GetItemFabric().GetIdTable().end();
                      ++it)
            {
                if ((*it) && ((*it)->RT_ITEM() == SpawnPoint::REAL_TYPE_ITEM))
                {
                    GetLobby().AddSpawnPoint((*it)->GetId());
                }
            }

            auto newmob = GetItemFabric().newItem<IMob>(LoginMob::T_ITEM_S());


            ChangeMob(newmob);
            GetItemFabric().SetPlayerId(id, newmob.ret_id());

            GetMapMaster()->FillAtmosphere();

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

        GetItemFabric().loadMap(ss, false, id);
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
        ss << "Hash: " << GetItemFabric().GetLastHash();
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

   /* GetTexts()["Connection"].SetUpdater
    ([this](std::string* str)
    {
        if (NetClient::GetNetClient()->IsFail())
            *str = "Connection lost";
        else
            *str = "";
    }).SetFreq(100).SetPlace(60, 0).SetSize(20).SetColor(250, 0, 0);*/


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

            size_t game_id = GetItemFabric().GetPlayerId(new_id);

            if (game_id != 0)
            {
                qDebug() << "game_id " << game_id << " already exists";
                continue;
            }

            auto newmob = GetItemFabric().newItem<IMob>(LoginMob::T_ITEM_S());

            qDebug() << "New client " << newmob.ret_id();

            GetItemFabric().SetPlayerId(new_id, newmob.ret_id());
            continue;
        }
        if (msg.type == MessageType::MAP_UPLOAD)
        {
            QJsonObject obj = Network2::ParseJson(msg);
            QString map_url = obj["url_to_upload_map"].toString();

            qDebug() << "Map upload to " << map_url;

            std::stringstream ss;
            GetItemFabric().saveMap(ss, false);
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
            size_t game_id = GetItemFabric().GetPlayerId(net_id);
            id_ptr_on<IMessageReceiver> game_object = game_id;
            game_object->processGUImsg(msg);
        }

        // TODO: other stuff
    }
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
