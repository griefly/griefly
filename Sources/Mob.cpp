#include "Mob.h"

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

#include "qtopengl.h"

#include <QCoreApplication>

int ping_send;

void Manager::checkMove(Dir direct)
{
    moveEach(direct);
};


void Manager::touchEach(Dir direct)
{
    GetMob()->dMove = direct;
}

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
    //GetQApp()->
/*    static Uint8* keys;
    int lastShoot = 0;
    int click_timer = 0;
    if (!NODRAW)
    {
        SDL_Event event;    
        while (SDL_PollEvent(&event))
        { 
            if(event.type == SDL_QUIT) 
                done = 1; 
            if(event.type == SDL_KEYUP)
            {
                if (event.key.keysym.sym == SDLK_o) 
                    pause = !pause;
                if (event.key.keysym.sym == SDLK_F12)
                    ToogleAutoplay();
                if(event.key.keysym.sym == SDLK_F2)
                {
                    ping_send = SDL_GetTicks();
                    Message msg;
                    msg.text = "SDLK_F2";
                    NetClient::GetNetClient()->Send(msg);
                }
            }
            if(event.type == SDL_MOUSEBUTTONDOWN)
            {
                if (event.button.button == SDL_BUTTON_WHEELUP)
                {
                    if (Chat::GetChat()->IsArea(event.button.x, event.button.y))
                        Chat::GetChat()->ScrollUp();
                }
                else if (event.button.button == SDL_BUTTON_WHEELDOWN)
                {
                    if (Chat::GetChat()->IsArea(event.button.x, event.button.y))
                        Chat::GetChat()->ScrollDown();
                }
                else if ((SDL_GetTicks() - click_timer) > 333)
                {
                    click_timer = SDL_GetTicks();
                    id_ptr_on<IOnMapObject> item;
                    if (GetMob()->GetInterface() && GetMob()->GetInterface()->IsArea(event.button.x, event.button.y))
                    {
                        item = GetMob()->GetInterface()->Click(event.button.x, event.button.y);
                    }
                    else
                    {
                        item = GetMapMaster()->click(event.button.x, event.button.y);
                    }
                    if (item)
                    {
                        Message msg;
                        msg.from = item.ret_id();
                        msg.text = "SDL_MOUSEBUTTONDOWN";
                        NetClient::GetNetClient()->Send(msg);
                        last_touch = item->name;
                    }
                }
  //              PlaySound("click.ogx");
            }
            if (event.type == SDL_VIDEORESIZE)
            {
                int max_scale = std::max((event.resize.w / 3), (event.resize.h / 2));

                int new_w = max_scale * 3;
                int new_h = max_scale * 2;
                GetScreen()->ResetScreen(new_w, new_h, 32, SDL_OPENGL | SDL_RESIZABLE);
            }
            if(event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_RETURN)
                {
                    if (!text_input_)
                        text_input_ = Chat::GetChat()->GetTextInput();
                    else
                    {
                        std::string str;
                        text_input_->GetText(&str);
                        if (str.size())
                        {
                            text_input_->Clean();
                        

                            SYSTEM_STREAM << str << std::endl;
                            text_input_ = nullptr;

                            Message msg;
                            msg.type = Net::CHAT_TYPE;
                            msg.text = str;
                            NetClient::GetNetClient()->Send(msg);
                        }
                    }
                }
                if (text_input_)
                {
                    LETTER_ADD(q);
                    LETTER_ADD(w);
                    LETTER_ADD(e);
                    LETTER_ADD(r);
                    LETTER_ADD(t);
                    LETTER_ADD(y);
                    LETTER_ADD(u);
                    LETTER_ADD(i);
                    LETTER_ADD(o);
                    LETTER_ADD(p);
                    LETTER_ADD(a);
                    LETTER_ADD(s);
                    LETTER_ADD(d);
                    LETTER_ADD(f);
                    LETTER_ADD(g);
                    LETTER_ADD(h);
                    LETTER_ADD(j);
                    LETTER_ADD(k);
                    LETTER_ADD(l);
                    LETTER_ADD(z);
                    LETTER_ADD(x);
                    LETTER_ADD(c);
                    LETTER_ADD(v);
                    LETTER_ADD(b);
                    LETTER_ADD(n);
                    LETTER_ADD(m);
                    LETTER_ADD(0);
                    LETTER_ADD(1);
                    LETTER_ADD(2);
                    LETTER_ADD(3);
                    LETTER_ADD(4);
                    LETTER_ADD(5);
                    LETTER_ADD(6);
                    LETTER_ADD(7);
                    LETTER_ADD(8);
                    LETTER_ADD(9);
                    if (event.key.keysym.sym == SDLK_SPACE)
                        { text_input_->AddLetter(" "); }
                    if (event.key.keysym.sym == SDLK_BACKSPACE)
                        { text_input_->AddLetter(nullptr); }
                    if (event.key.keysym.sym == SDLK_LEFT)
                        { text_input_->PointerLeft(); }
                    if (event.key.keysym.sym == SDLK_RIGHT)
                        { text_input_->PointerRight(); }
                    if (event.key.keysym.sym == SDLK_DELETE)
                        { text_input_->Clean(); }
                }
                else if (event.key.keysym.sym == SDLK_s)
                {
                    Message msg;
                    msg.text = "SDLK_s_down";
                    NetClient::GetNetClient()->Send(msg);
                    lastShoot = SDL_GetTicks();
                }
            }
        }
    }

    if (text_input_)
        return;

    SDL_PumpEvents();
    keys = SDL_GetKeyState(NULL);
    /*if(keys[SDLK_h])
    {
        int locatime = SDL_GetTicks();
        auto itr = map->squares[thisMob->posx][thisMob->posy].begin();
        int i = 0;
        while(itr != map->squares[thisMob->posx][thisMob->posy].end())
        {
            SYSTEM_STREAM << i <<": Level " << (*itr)->level;
            itr++;
            i++;
        };
        SYSTEM_STREAM << "Num item: " << i << " in " << (SDL_GetTicks() - locatime) * 1.0 / 1000 << " sec" << std::endl;
    }
    if(keys[SDLK_F5])
    {
        int locatime = SDL_GetTicks();
        GetItemFabric()->saveMap("clientmap.map");
        SYSTEM_STREAM << "Map saved in "<< (SDL_GetTicks() - locatime) * 1.0 / 1000 << " second" << std::endl;
    }
    if(keys[SDLK_F6])
    {
        int locatime = SDL_GetTicks();
        GetItemFabric()->clearMap();
        GetItemFabric()->loadMap("clientmap.map");
        SYSTEM_STREAM << "Map load in " << (SDL_GetTicks() - locatime) * 1.0 / 1000 << " second" << std::endl;
    }
    if(keys[SDLK_h])
    {
        SYSTEM_STREAM << "World's hash: " << GetItemFabric()->hash_all() << std::endl; 
    }

    SEND_KEY_MACRO(SDLK_UP);
    SEND_KEY_MACRO(SDLK_DOWN);
    SEND_KEY_MACRO(SDLK_LEFT);
    SEND_KEY_MACRO(SDLK_RIGHT);
    SEND_KEY_MACRO(SDLK_p);

    SEND_KEY_MACRO(SDLK_z);
    SEND_KEY_MACRO(SDLK_x);
    SEND_KEY_MACRO(SDLK_c);j
    SEND_KEY_MACRO(SDLK_v);
    */
}

void Manager::ProcessClick(int mouse_x, int mouse_y)
{
    if (GetMob()->GetInterface() && GetMob()->GetInterface()->Click(mouse_x, mouse_y))
    {
        last_touch = "Interface";
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
    if ((event->key() == Qt::Key_8) && (event->modifiers() == Qt::KeypadModifier))
    {
        msg.text = Input::MOVE_UP;
    }
    else if ((event->key() == Qt::Key_2) && (event->modifiers() == Qt::KeypadModifier))
    {
        msg.text = Input::MOVE_DOWN;
    }
    else if ((event->key() == Qt::Key_6) && (event->modifiers() == Qt::KeypadModifier))
    {
        msg.text = Input::MOVE_RIGHT;
    }
    else if ((event->key() == Qt::Key_4) && (event->modifiers() == Qt::KeypadModifier))
    {
        msg.text = Input::MOVE_LEFT;
    }
    else if (event->key() == Qt::Key_Up)
    {
        msg.text = Input::MOVE_UP;
    }
    else if (event->key() == Qt::Key_Down)
    {
        msg.text = Input::MOVE_DOWN;
    }
    else if (event->key() == Qt::Key_Right)
    {
        msg.text = Input::MOVE_RIGHT;
    }
    else if (event->key() == Qt::Key_Left)
    {
        msg.text = Input::MOVE_LEFT;
    }
    else
    {
        return;
    }
    NetClient::GetNetClient()->Send(msg);
}

void Manager::initWorld()
{
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
                hash("Human"), 
                GetMapMaster()->squares[GetMapMaster()->GetMapW() / 2]
                                       [GetMapMaster()->GetMapH() / 2]
                                       [GetMapMaster()->GetMapD() / 2]);
        ChangeMob(newmob);
        GetItemFabric()->SetPlayerId(newmob.ret_id(), newmob.ret_id());

        auto tptr = GetItemFabric()->newItemOnMap<IOnMapObject>(
                hash("Teleportator"), 
                GetMapMaster()->squares[GetMapMaster()->GetMapW() / 2]
                                       [GetMapMaster()->GetMapH() / 2]
                                       [GetMapMaster()->GetMapD() / 2]);
        GetMapMaster()->FillAtmosphere();
        SetCreator(tptr.ret_id());
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
                hash("Human"), 
                GetMapMaster()->squares[GetMapMaster()->GetMapW() / 2]
                                       [GetMapMaster()->GetMapH() / 2]
                                       [GetMapMaster()->GetMapD() / 2]);
        ChangeMob(newmob);
        GetItemFabric()->SetPlayerId(newmob.ret_id(), newmob.ret_id());

        auto tptr = GetItemFabric()->newItemOnMap<IOnMapObject>(
                hash("Teleportator"), 
                GetMapMaster()->squares[GetMapMaster()->GetMapW() / 2]
                                       [GetMapMaster()->GetMapH() / 2]
                                       [GetMapMaster()->GetMapD() / 2]);
        GetMapMaster()->FillAtmosphere();
        SetCreator(tptr.ret_id());
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


