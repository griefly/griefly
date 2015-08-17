#include "Manager.h"

#include <memory>

#include "MapClass.h"
#include "Turf.h"
#include "Text.h"

#include "mob_position.h"

#include "NetClientImpl.h"
#include "EffectSystem.h"
#include "MoveEffect.h"
#include "sync_random.h"
#include "ItemFabric.h"
#include "MagicStrings.h"
#include "TileInt.h"
#include "Debug.h"
#include "Params.h"

#include "sound.h"
#include "Creator.h"
#include "SdlInit.h"
#include "MobInt.h"
#include "utils.h"
#include "ImageLoader.h"
#include "SoundLoader.h"
#include "Chat.h"
#include "Names.h"
#include "IMovable.h"
#include "Human.h"

#include "AutogenMetadata.h"

#include "qtopengl.h"

#include <QCoreApplication>

int ping_send;

void Manager::checkMove(Dir direct)
{
    moveEach(direct);
};

void Manager::moveEach(Dir direct)
{
    undoCenterMove(direct);
};

void Manager::undoCenterMove(Dir direct)
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
};

Manager::Manager(std::string adrs)
{
    adrs_ = adrs;
    auto_player_ = false;
    visiblePoint = new std::list<point>;
    isMove = false;
    done = 0;
    pause = false;
    last_fps = FPS_MAX;
};

void Manager::UpdateVisible() 
{
    visiblePoint->clear();

    GetMob()->CalculateVisible(visiblePoint);

    //visiblePoint = 
    //    GetMapMaster()->losf.calculateVisisble(visiblePoint, 
    //        GetMob()->GetX(), 
    //        GetMob()->GetY(),
    //        GetMob()->GetZ());
}

void Manager::process()
{
    GetMapMaster()->numOfPathfind = 0;
    SDL_Color color = {255, 255, 255, 0};

    int begin_of_process;

    int delay = 0;
    int lastTimeFps = SDL_GetTicks();
    int lastTimeC   = SDL_GetTicks();
    fps = 0;
    bool process_in = false;
    Timer tick_timer, draw_timer, process_timer, atmos_move_timer, force_timer;
    tick_timer.Start();
    draw_timer.Start();
    process_timer.Start();
    atmos_move_timer.Start();
    force_timer.Start();
    unsigned int draw_time_per_tick = 0;
    unsigned int process_time_per_tick = 0;

    unsigned int last_effect_process = 0;
    static int locTime = 0;
    while(done == 0)
    { 

        processInput();
        if(NetClient::GetNetClient()->Ready() && !pause)
        {
            process_in = true;
            process_in_msg();
            MAIN_TICK++;
        }

        const int ATMOS_OFTEN = 1;
        const int ATMOS_MOVE_OFTEN = 1;

        if(process_in && !pause)
        {
            process_timer.Start();
            numOfDeer = 0;
            begin_of_process = SDL_GetTicks();
            GetItemFabric()->foreachProcess();
            force_timer.Start();
            ForceManager::Get().Process();
            unsigned int fm = force_timer.Get();
            if (ATMOS_OFTEN == 1 || MAIN_TICK % ATMOS_OFTEN == 1)
                GetMapMaster()->atmosphere.Process();
            atmos_move_timer.Start();
            if (ATMOS_MOVE_OFTEN == 1 || MAIN_TICK % ATMOS_MOVE_OFTEN == 1)
                GetMapMaster()->atmosphere.ProcessMove();
            unsigned int amt = atmos_move_timer.Get();
            GetItemFabric()->Sync();
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

            Chat::GetChat()->Process();

            GetMob()->processGUI();
            
            GetTexts().Process();
            
            //glFinish();
            GetScreen()->Swap();
            draw_time_per_tick += draw_timer.Get();
        }

        if((SDL_GetTicks() - lastTimeFps) >= 1000 && !pause)
        {
            UpdateVisible();

            if(!(fps > FPS_MAX - 10 && fps < FPS_MAX - 10))
            delay = (int)(1000.0 / FPS_MAX + delay - 1000.0 / fps);
            lastTimeFps = SDL_GetTicks();
            last_fps = fps;
            fps = 0;
          

            GetMapMaster()->numOfPathfind = 0;
        }
        ++fps;
        process_in = false;
        if (NetClient::GetNetClient()->Process() == false)
        {

        }

        Debug::UnsyncDebug().ProcessDebug();

        if(SDL_GetTicks() - locTime > 100)
        {
            if (GetItemFabric()->get_hash_last() == NetClient::GetNetClient()->Hash())
                GetTexts()["Sync"].SetColor(0, 255, 100);
            else
                GetTexts()["Sync"].SetColor(255, 160, 0);
        }
        if (GetMainWidget()->isHidden())
        {
            break;
        }
    }
}

void Manager::ClearGUIZone()
{
    /*glColor3f(0.8f, 0.8f, 0.8f);
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
        glVertex2i(sizeW,                0);
        glVertex2i(sizeW,            sizeH);
        glVertex2i(sizeW + guiShift, sizeH);
        glVertex2i(sizeW + guiShift,     0);
    glEnd();
    glEnable(GL_TEXTURE_2D);*/
}

void Manager::checkMoveMob()
{
};

#define SEND_KEY_MACRO(key) \
      if((auto_player_ && (rand() % 100 == 1)) || (!NODRAW && keys[key])) \
      { \
          if(SDL_GetTicks() - lastShoot >= 80) \
          { \
              Message msg; \
              msg.text = #key; \
              NetClient::GetNetClient()->Send(msg); \
              lastShoot = SDL_GetTicks(); \
          } \
      }

#define LETTER_ADD(letter) \
    if (event.key.keysym.sym == SDLK_##letter) \
        { text_input_->AddLetter(#letter); }

void Manager::processInput()
{
    GetQApp()->processEvents(QEventLoop::AllEvents, 40);
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
        last_touch = "Interface";
        return;
    }

    // TODO: Remake, huge exploit
    if (id_ptr_on<Human> h = GetMob())
    {
        if (h->GetLying() == true)
            return;
    }

    id_ptr_on<IOnMapObject> item;
    item = GetMapMaster()->click(mouse_x, mouse_y);
    if (item)
    {
        Message msg;
        msg.from = item.ret_id();
        msg.text = Input::LEFT_CLICK;
        NetClient::GetNetClient()->Send(msg);
        last_touch = item->name;
    }
}

void Manager::HandleKeyboardDown(QKeyEvent* event)
{
    Message msg;

    int val = rand();

    if ((!event && (val % 100 == 1)) || (event && event->key() == Qt::Key_8) && (event->modifiers() == Qt::KeypadModifier))
    {
        msg.text = Input::MOVE_UP;
    }
    else if ((!event && (val % 100 == 2)) || (event && event->key() == Qt::Key_2) && (event->modifiers() == Qt::KeypadModifier))
    {
        msg.text = Input::MOVE_DOWN;
    }
    else if ((!event && (val % 100 == 3)) || (event && event->key() == Qt::Key_6) && (event->modifiers() == Qt::KeypadModifier))
    {
        msg.text = Input::MOVE_RIGHT;
    }
    else if ((!event && (val % 100 == 4)) || (event && event->key() == Qt::Key_4) && (event->modifiers() == Qt::KeypadModifier))
    {
        msg.text = Input::MOVE_LEFT;
    }
    else if ((!event && (val % 100 == 5)) || (event && event->key() == Qt::Key_Up))
    {
        msg.text = Input::MOVE_UP;
    }
    else if ((!event && (val % 100 == 6)) || (event && event->key() == Qt::Key_Down))
    {
        msg.text = Input::MOVE_DOWN;
    }
    else if ((!event && (val % 100 == 7)) || (event && event->key() == Qt::Key_Right))
    {
        msg.text = Input::MOVE_RIGHT;
    }
    else if ((!event&& (val % 100 == 8)) || (event && event->key() == Qt::Key_Left))
    {
        msg.text = Input::MOVE_LEFT;
    }
    else if ((!event && (val % 100 == 9)) || (event && event->key() == Qt::Key_Q))
    {
        msg.text = Input::KEY_Q;
    }
    else if ((!event && (val % 100 == 10)) || (event && event->key() == Qt::Key_W))
    {
        msg.text = Input::KEY_W;
    }
    else if ((!event && (val % 100 == 11)) || (event && event->key() == Qt::Key_E))
    {
        msg.text = Input::KEY_E;
    }
    else if ((!event && (val % 100 == 12)) || (event && event->key() == Qt::Key_R))
    {
        msg.text = Input::KEY_R;
    }
    else if (/* NO! */ (event && event->key() == Qt::Key_F3))
    {
        Debug::UnsyncDebug().GenerateAndSaveReport();
        return;
    }
    else
    {
        return;
    }
    NetClient::GetNetClient()->Send(msg);
}

void Manager::initWorld()
{
    if (!GetParamsHolder().GetParamBool("-debug_to_chat"))
    {
        SetLogToFile();
    }


    InitSettersForTypes();

    std::cout << "Begin init world" << std::endl;
    tick_recv = 0;
    isMove = 0;

    if (!InitSDL())
    {
        SYSTEM_STREAM << "Fail SDL load" << std::endl;
    }
    //SDL_WM_SetCaption(Debug::GetUniqueName().c_str(), Debug::GetUniqueName().c_str());

    std::cout << "Begin set manager" << std::endl;
    SetManager(this);

    //sizeW = GetGLWidget()->width();
    //sizeH = GetGLWidget()->height();

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
    SetSpriter(new ASprClass);

    std::cout << "Begin load resources" << std::endl;
    LoadImages();
    LoadSounds();
    LoadNames();

    std::cout << "Net init" << std::endl;
    NetClient::Init();

    std::cout << "Create tiles" << std::endl;
    int x = GetParamsHolder().GetParamBool("map_x") ? GetParamsHolder().GetParam<int>("map_x") : 40;
    int y = GetParamsHolder().GetParamBool("map_y") ? GetParamsHolder().GetParam<int>("map_y") : 40;
    int z = GetParamsHolder().GetParamBool("map_z") ? GetParamsHolder().GetParam<int>("map_z") : 1;
    GetMapMaster()->makeTiles(x, y, z);

    std::cout << "Begin choose map" << std::endl;
    if (   GetParamsHolder().GetParamBool("mapgen_name")
        && utils::IsFileExist(GetParamsHolder().GetParam<std::string>("mapgen_name")))
    {
        GetMapMaster()->LoadFromMapGen(GetParamsHolder().GetParam<std::string>("mapgen_name"));

        auto newmob = GetItemFabric()->newItemOnMap<IMob>(
                Human::T_ITEM_S(),
                GetMapMaster()->squares[GetMapMaster()->GetMapW() / 2]
                                       [GetMapMaster()->GetMapH() / 2]
                                       [GetMapMaster()->GetMapD() / 2]);
        ChangeMob(newmob);
        GetItemFabric()->SetPlayerId(newmob.ret_id(), newmob.ret_id());

        GetMapMaster()->FillAtmosphere();

        GetItemFabric()->SetPlayerId(newmob.ret_id(), newmob.ret_id());
        srand(SDL_GetTicks());
    }
    else if 
       (   GetParamsHolder().GetParamBool("map_name") 
         && utils::IsFileExist(GetParamsHolder().GetParam<std::string>("map_name")))
    {
       std::string str = GetParamsHolder().GetParam<std::string>("map_name");
       GetItemFabric()->loadMap(str.c_str());
    }
    else
    {
        std::cout << "Begin create map" << std::endl;
        auto newmob = GetItemFabric()->newItemOnMap<IMob>(
                Human::T_ITEM_S(),
                GetMapMaster()->squares[GetMapMaster()->GetMapW() / 2]
                                       [GetMapMaster()->GetMapH() / 2]
                                       [GetMapMaster()->GetMapD() / 2]);
        ChangeMob(newmob);
        GetItemFabric()->SetPlayerId(newmob.ret_id(), newmob.ret_id());

        GetMapMaster()->FillAtmosphere();

        GetItemFabric()->SetPlayerId(newmob.ret_id(), newmob.ret_id());
        srand(SDL_GetTicks());
        GetMapMaster()->makeMap();
    }

    LoginData data;
    data.who = GetMob().ret_id();
    data.word_for_who = 1;
    NetClient::GetNetClient()->Connect(adrs_, DEFAULT_PORT, data);

    Chat::InitChat();

    GetTexts()["FPS"].SetUpdater
    ([this](std::string* str)
    {
        std::stringstream ss; 
        ss << last_fps - 1; 
        ss >> *str;
    }).SetFreq(1000).SetSize(20);

    GetTexts()["Sync"].SetUpdater
    ([&](std::string* str)
    {
        std::stringstream ss;
        ss << ((GetItemFabric()->get_hash_last() == NetClient::GetNetClient()->Hash()) ? "SYNC:" : "UNSYNC:")
           << GetItemFabric()->get_hash_last();
        ss >> *str;
    }).SetSize(15).SetPlace(0, 30, 200, 50);

    GetTexts()["MorePreciseSync"].SetUpdater
    ([&](std::string* str)
    {
        std::stringstream ss;
        if (Debug::UnsyncDebug().IsReportGenerated())
        {
            ss << "!!REPORT!!";
        }
        ss >> *str;
    }).SetSize(15).SetPlace(200, 30).SetColor(200, 0, 0);

    GetTexts()["SyncTick"].SetUpdater
    ([&](std::string* str)
    {
        std::stringstream ss;
        ss << NetClient::GetNetClient()->HashTick();
        ss >> *str;
    }).SetSize(15).SetPlace(120, 0).SetColor(150, 0, 0);

    GetTexts()["PingTimer"].SetUpdater
    ([&](std::string* str)
    {
        std::stringstream ss;
        ss << "Ping: " << NetClient::GetNetClient()->Ping() << "ms";
        *str = ss.str();
    }).SetSize(15).SetPlace(300, 0).SetColor(0, 140, 0);

    GetTexts()["LastTouch"].SetUpdater
    ([this](std::string* str)
    {
        *str = last_touch;
    }).SetFreq(20).SetPlace(0, 485).SetSize(22);

    GetTexts()["Connection"].SetUpdater
    ([this](std::string* str)
    {
        if (NetClient::GetNetClient()->IsFail())
            *str = "Connection lost";
        else
            *str = "";
    }).SetFreq(100).SetPlace(60, 0).SetSize(20).SetColor(250, 0, 0);


}

void Manager::loadIniFile()
{

}

void Manager::process_in_msg()
{
    Message msg;
    while (true)
    {
        NetClient::GetNetClient()->Recv(&msg);

        if (msg.text == Net::MAKE_NEW)
        {
            auto newmob = GetItemFabric()->newItemOnMap<IMob>(
                        Human::T_ITEM_S(),
                        GetMapMaster()->squares[GetMapMaster()->GetMapW() / 2]
                                               [GetMapMaster()->GetMapH() / 2]
                                               [GetMapMaster()->GetMapD() / 2]);
            SYSTEM_STREAM << "NEW MOB CREATE HIS ID " << newmob.ret_id() << "\n";
            //newmob->onMobControl = true;

            GetItemFabric()->SetPlayerId(newmob.ret_id(), newmob.ret_id());

            Message msg_new;
            msg_new.from = msg.from;
            msg_new.to = newmob.ret_id();
            msg_new.type = Net::SYSTEM_TYPE;
            msg_new.text = Net::NEW_MAKED;

            NetClient::GetNetClient()->Send(msg_new);
            continue;
        }

        if (msg.text == "SDLK_F2")
        {
            SYSTEM_STREAM << "Ping is: " << (SDL_GetTicks() - ping_send) / 1000.0 << "s" << std::endl;
            continue;
        }
        if (msg.text == Net::NEXTTICK)
            return;
        
        id_ptr_on<IMessageReceiver> i;
        i = msg.to;

        if (i.valid())
            i->processGUImsg(msg);
        else
            SYSTEM_STREAM << "Wrong id accepted - " << msg.to << std::endl;
    }
}

bool Manager::isMobVisible(int posx, int posy)
{
    // TODO: matrix for fast check
    if (visiblePoint == nullptr)
        return false;
    for (auto it = visiblePoint->begin(); it != visiblePoint->end(); ++it)
        if(it->posx == posx && it->posy == posy)
            return true;
    return false;
}

QApplication* app = nullptr;
QApplication* GetQApp()
{
    return app;
}

void SetQApp(QApplication* new_app)
{
    app = new_app;
}

QWidget* main_widget = nullptr;
QWidget* GetMainWidget()
{
    return main_widget;
}

void SetMainWidget(QWidget* widget)
{
    main_widget = widget;
}

Manager* manager_ = nullptr;
Manager* GetManager()
{
    return manager_;
}

void SetManager(Manager* manager)
{
    manager_ = manager;
}


